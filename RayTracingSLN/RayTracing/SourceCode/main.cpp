#include "geometry.h"
#include "LoadObj.h"
#include "Model.h"
#include "RayCaster.h"
#include "platform.h"
#include "our_gl.h"
#include "Shader.h"
#include "GouradShader.h"


static const char* const WINDOW_TITLE = "RayTracing";
static const int WINDOW_WIDTH = 500;
static const int WINDOW_HEIGHT = 500;
static const int WINDOW_TEXT_WIDTH =50;
static const int WINDOW_TEXT_HEIGHT = 70;
int scene_count = 2;
//SceneInfo scene_info;
RenderBuffer* frame_buffer = nullptr;

extern mat<4, 4> ModelView; // "OpenGL" state matrices
extern mat<4, 4> Projection;

#pragma region 灯光和材质和MVP矩阵
 vec3 light_dir{ 100,100,0 }; // light source
constexpr vec3       eye{ 1,1,3 }; // camera position
constexpr vec3    center{ 0,0,0 }; // camera direction
constexpr vec3        up{ 0,1,0 }; // camera up vector



std::vector<vec3> lights = std::vector<vec3>{
	{-20, 20,  20},
	{ 30, 50, -25},
};

std::vector<Model> models;
constexpr Material      ivory = { 1.0, {0.9,  0.5, 0.1, 0.0}, {0.4 , 0.4 , 0.3 },   50. };
constexpr Material      glass = { 1.5, {0.0,  0.9, 0.1, 0.8}, {0.6 , 0.7 , 0.8 },  125. };
constexpr Material red_rubber = { 1.0, {1.4,  0.3, 0.0, 0.0}, {0.3 , 0.1 , 0.1 },   10. };
constexpr Material     mirror = { 1.0, {0.0, 16.0, 0.8, 0.0}, {1.0 , 1.0 , 1.0 }, 1425. };
#pragma endregion

std::vector<std::string> path = std::vector<std::string>{ "../obj/african_head.obj" };
#pragma region 模型加载
void LoadModel()
{
    LoadObj model(path[0]);
    std::vector<std::vector<vec3 >> world_coords;
    // First model
    for (int i = 0; i < model.nfaces(); ++i) {
        std::vector<vec3 > currTri;
        for (int j : {0, 1, 2}) {
            vec3  vertex = model.vert(i, j);
            vertex.x *= 0.3;
            vertex.y *= 0.3;
            vertex.z *= 0.3;
            vertex.x += 0.1;
            vertex.z -= 1;
            currTri.push_back(vertex);
        }
        world_coords.push_back(currTri);
    }
    models.emplace_back(world_coords, red_rubber);

    //world_coords.clear();
    //// First model
    //for (int i = 0; i < model.nfaces(); ++i) {
    //    std::vector<vec3 > currTri;
    //    for (int j : {0, 1, 2}) {
    //        vec3  vertex = model.vert(i, j);
    //        vertex.x *= 0.3;
    //        vertex.y *= 1;
    //        vertex.z *= 1;
    //        vertex.z -= 1;
    //        vertex.x -= 0.5;
    //        currTri.push_back(vertex);
    //    }
    //    world_coords.push_back(currTri);
    //}
    //models.emplace_back(world_coords, mirror);

    //world_coords.clear();
    //// First model
    //for (int i = 0; i < model.nfaces(); ++i) {
    //    std::vector<vec3 > currTri;
    //    for (int j : {0, 1, 2}) {
    //        vec3  vertex = model.vert(i, j);
    //        vertex.x *= 0.2;
    //        vertex.y *= 0.2;
    //        vertex.z *= 0.2;
    //        vertex.z -= 1;
    //        vertex.x -= 0.1;
    //        vertex.y -= 0.3;
    //        currTri.push_back(vertex);
    //    }
    //    world_coords.push_back(currTri);
    //}
    //models.emplace_back(world_coords, ivory);
}
#pragma endregion

#pragma region  光线追踪函数 
void CalculatePerPix(float fov,vec3 eye)
{
	// 清除颜色缓存和深度缓存
	frame_buffer->renderbuffer_clear_color(Color::Black);
	frame_buffer->renderbuffer_clear_depth(std::numeric_limits<float>::max());

#pragma omp parallel for
	for (int pix = 0; pix < WINDOW_WIDTH * WINDOW_HEIGHT; pix++) { // actual rendering loop
		float dir_x = (pix % WINDOW_WIDTH + 0.5) - WINDOW_WIDTH / 2-eye.x;
		float dir_y = eye.y-(pix / WINDOW_WIDTH + 0.5) + WINDOW_HEIGHT / 2 ; // this flips the image at the same time
		float dir_z = eye.z -WINDOW_HEIGHT / (2. * tan(fov / 2.)) ;
		frame_buffer->set_color0_1(pix % WINDOW_WIDTH, pix / WINDOW_WIDTH, cast_ray(vec3{ 0,0,0 }, vec3{ dir_x, dir_y, dir_z }.normalized(), models, lights));
		//frame_buffer->set_color(pix % WINDOW_WIDTH, pix / WINDOW_WIDTH, vec3{ 0,0,0 });
	}
}
#pragma endregion



int main()
{
// build the Projection matrix
frame_buffer = new RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

#pragma region  光线追踪（可选） 
	//LoadModel();
	//float fov = 1.2; // 60 degrees field of view in radians
	//vec3 eye = vec3{ 10, 0,  0 };
	//CalculatePerPix(fov, eye);
#pragma endregion

#pragma region  软光栅（可选） 
	lookat(eye, center, up);                            // build the ModelView matrix
	viewport(WINDOW_WIDTH / 8, WINDOW_HEIGHT / 8, WINDOW_WIDTH * 3 / 4, WINDOW_HEIGHT * 3 / 4); // build the Viewport matrix
	projection((eye - center).norm()); // build the Projection matrix
	LoadObj model(path[0]);
	std::vector<double> zbuffer(WINDOW_WIDTH * WINDOW_HEIGHT, std::numeric_limits<double>::max());
	GouradShader shader(model, light_dir.normalized(), ModelView, Projection);
		for (int i = 0; i < model.nfaces(); i++) { // for every triangle
			vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
			for (int j : {0, 1, 2})
				shader.vertex(i, j, clip_vert[j]); // call the vertex shader for each triangle vertex

			triangle(clip_vert, shader, *frame_buffer, zbuffer); // actual rasterization routine call
		}
#pragma endregion


#pragma region  初始化GUI界面
	platform_initialize();
	window_t* window;
	//Record record = Record();
	callbacks_t callbacks = callbacks_t();
	float prev_time = platform_get_time();
	float print_time = prev_time;
	int num_frames = 0;
	const int text_size = 500;
	char screen_text[text_size];
	int show_num_frames = 0;
	int show_avg_millis = 0;
	float refresh_screen_text_timer = 0;
	const float REFRESH_SCREEN_TEXT_TIME = 0.1;
	snprintf(screen_text, text_size, "fps: - -, avg: - -ms\n");
	window = window_create(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TEXT_WIDTH, WINDOW_TEXT_HEIGHT);
	//frame_buffer = new RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
#pragma endregion

	while (!window_should_close(window)) {
		float curr_time = platform_get_time();
		float delta_time = curr_time - prev_time;

		// 把帧缓存绘制到UI窗口
		window_draw_buffer(window, frame_buffer);
#pragma region 交互事件
		if (input_key_pressed(window, KEY_W)) {
			lights[0].x += 3;
		}
		
#pragma endregion

#pragma region 文本显示
		// 计算帧率和耗时
		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;

			show_num_frames = num_frames;
			show_avg_millis = avg_millis;
			num_frames = 0;
			print_time = curr_time;
		}
		prev_time = curr_time;
		//更新显示文本信息
		refresh_screen_text_timer += delta_time;
		if (refresh_screen_text_timer > REFRESH_SCREEN_TEXT_TIME)
		{
			snprintf(screen_text, text_size, "");

			char line[50] = "";

			snprintf(line, 50, "fps: %3d, avg: %3d ms\n\n", show_num_frames, show_avg_millis);
			strcat(screen_text, line);

			//snprintf(line, 50, "scene: %s\n", scene_info.name);
			strcat(screen_text, line);
			snprintf(line, 50, "press key [W] or [S] to switch scene\n\n");
			strcat(screen_text, line);

			//strcat(screen_text, scene_info.scene->get_text());

			window_draw_text(window, screen_text);
			refresh_screen_text_timer -= REFRESH_SCREEN_TEXT_TIME;
		}
#pragma endregion
		input_poll_events();
	}


	delete frame_buffer;
	window_destroy(window);
}