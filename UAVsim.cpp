#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

struct Car {
	double x, y; // Koordinatlar fmod ve küsuratlı işlemler için double'a çevrildi
	double vx_base, vy_base;
	int selected_car_index;
	double time_offset;
};

int g_selected_mode = -1; //Global değişken tanımladık.

void onMouse(int event, int x, int y, int flags, void* userdata) {

	if (event == cv::EVENT_LBUTTONDOWN) {
		// 1. Butonunu koordinatları.
		if (x >= 100 && x <= 500 && y >= 100 && y <= 160) {  //Only Noise butonu.
			g_selected_mode = 0;
		} // 2. Butonun koordinatları.
		else if (x >= 100 && x <= 500 && y >= 200 && y <= 260) {  //Noise + Termal Negatif butonu.
			g_selected_mode = 1;
		}
	}
}

int runMenu() {
	//SEÇİM PENCERESİ
	cv::Mat menu = cv::Mat::zeros(400, 600, CV_8UC3);  // 400x600 lük bir siyah matris oluşturur.
	cv::namedWindow("MENU");//MENU başlıklı pencere açar.
	cv::setMouseCallback("MENU", onMouse); //Bu pencerede mouse hareketlerini dinleme görevi verir. onMouse fonksiyonu.

	cv::putText(menu, "Simulation Panel", cv::Point(120, 60), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 255), 2);

	// 1. BUTON
	cv::rectangle(menu, cv::Point(100, 100), cv::Point(500, 160), cv::Scalar(100, 100, 100), -1);
	std::string text1 = "1. Only Noise";
	int baseline1 = 0;
	cv::Size textSize1 = cv::getTextSize(text1, cv::FONT_HERSHEY_SIMPLEX, 0.7, 2, &baseline1);
	cv::putText(menu, text1, cv::Point(300 - textSize1.width/2, 140), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

	// 2. BUTON
	cv::rectangle(menu, cv::Point(100, 200), cv::Point(500, 260), cv::Scalar(100, 100, 100), -1);
	std::string text2 = "2. NOISE + NEGATIVE THERMAL";
	int baseline2 = 0;
	cv::Size textSize2 = cv::getTextSize(text2, cv::FONT_HERSHEY_SIMPLEX, 0.7, 2, &baseline2);
	cv::putText(menu, text2, cv::Point(300 - textSize2.width/2, 240), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
	
	while (g_selected_mode == -1) {  //Seçim yapılana kadar bekle.

		cv::imshow("MENU", menu); //Menüyü gösterir.
		
		if (cv::waitKey(10) == 27) exit(0); //ESC ile çıkış.

	}
	
	cv::destroyWindow("MENU");
	return g_selected_mode;
}

int main() {

	std::srand(static_cast<unsigned int>(std::time(0))); // Rastgele sayı üretici.

	int filter_mode = runMenu(); 

	cv::Mat src = cv::imread("setallite_img.png"); // Uydu görüntüsünü okundu.

	if (src.empty()) {
		std::cout << "Setallite image is not found." << std::endl;
		return -1;
	}

	cv::Mat row_patterns[3] = { src, src, src }; // Uydu görüntüsünü yan yana 3 defa dizerek satır oluşturldu.
	cv::Mat single_row;
	cv::hconcat(row_patterns, 3, single_row);

	cv::Mat grid_rows[3] = { single_row, single_row, single_row }; // Yan yana 3 görüntü koyulmuş satırdan alt alta 3 tane koyarak 3x3 lük matris görüntü elde edilir.
	cv::Mat final_mosaic;
	cv::vconcat(grid_rows, 3, final_mosaic);

	int map_height = final_mosaic.rows;
	int map_width = final_mosaic.cols;

	cv::Mat double_mosaic_row;
	cv::Mat mosaic_pairs[2] = { final_mosaic, final_mosaic };
	cv::hconcat(mosaic_pairs, 2, double_mosaic_row);

	cv::Mat clean_map = double_mosaic_row.clone();

	cv::Mat car1 = cv::imread("camauflage_car.png", cv::IMREAD_UNCHANGED);
	cv::Mat car2 = cv::imread("camauflage_car1.png", cv::IMREAD_UNCHANGED); // Araba görselleri okundu.

	if (car1.empty() || car2.empty()) {
		std::cout << "Car images are not found." << std::endl;
		return -1;
	}

	cv::Mat car1_original = car1.clone();
	cv::Mat car2_original = car2.clone();

	Car cars[5];  

	for (int i = 0; i < 5; ++i) {

		cars[i].selected_car_index = std::rand() % 2;
		
		int initial_w = 60;
		int initial_h = 90;

		cars[i].x = std::rand() % (map_width - initial_w);
		cars[i].y = std::rand() % (map_height - initial_h);

		cars[i].vx_base = (std::rand() % 7) + 3;
		cars[i].vy_base = (std::rand() % 7) + 3;

		if (std::rand() % 2 == 0) cars[i].vx_base *= -1;
		if (std::rand() % 2 == 0) cars[i].vy_base *= -1;

		cars[i].time_offset = static_cast<double>(std::rand()) / RAND_MAX * 100.0;

	}

	cv::namedWindow("Simulation", cv::WINDOW_NORMAL); //Simulation başlıklı pencere açar.

	int noise_intensity = 15;
	int shake_intensity = 12;
	int car_size_scale = 30; 
	cv::createTrackbar("Noise Density", "Simulation", &noise_intensity, 100);
	cv::createTrackbar("Shake Intensity", "Simulation", &shake_intensity, 100);
	cv::createTrackbar("Car Size", "Simulation", &car_size_scale, 100);

	long long frame_count = 0;
	double map_scroll_x = 0.0;
	double scroll_speed = 5.0;

	while (true) {

		cv::Mat frame = clean_map.clone();
		frame_count++;

		map_scroll_x = std::fmod(map_scroll_x + scroll_speed, static_cast<double>(map_width));

		int shake_offset_x = 0;

		double s_amp1 = static_cast<double>(shake_intensity);
		double s_amp2 = static_cast<double>(shake_intensity) * 0.33;
		int shake_offset_y = static_cast<int>(std::sin(frame_count * 8.0) * s_amp1 + std::cos(frame_count * 16.0) * s_amp2);

		int base_scroll_x = static_cast<int>(map_scroll_x);
		int visible_x = (base_scroll_x + shake_offset_x + map_width) % map_width;

		int visible_y = (shake_offset_y % map_height + map_height) % map_height;

		cv::Rect visible_area(visible_x, 0, map_width, map_height);
		cv::Mat base_strip = clean_map(visible_area).clone();

		cv::Mat scroll_frame = cv::Mat::zeros(map_height, map_width, clean_map.type());

		int s_y = visible_y % map_height;
		cv::Rect crop_source(0, s_y, map_width, map_height - s_y);
		cv::Rect target_roi(0, 0, map_width, map_height - s_y);

		if (crop_source.height > 0 && crop_source.y + crop_source.height <= base_strip.rows) {
			base_strip(crop_source).copyTo(scroll_frame(target_roi));
		}
		if (s_y > 0) {
			cv::Rect wrap_source(0, 0, map_width, s_y);
			cv::Rect wrap_target(0, map_height - s_y, map_width, s_y);
			base_strip(wrap_source).copyTo(scroll_frame(wrap_target));
		}

		int current_base_w = std::max(10, car_size_scale * 2);
		int current_base_h = std::max(15, static_cast<int>(car_size_scale * 3.0)); 

		cv::Size dynamic_car_size(current_base_w, current_base_h);
		cv::Mat car1_resized, car2_resized;
		cv::resize(car1_original, car1_resized, dynamic_car_size);
		cv::resize(car2_original, car2_resized, dynamic_car_size);

		cv::Mat car_images[2] = { car1_resized, car2_resized };

		for (int i = 0; i < 5; ++i) {

			cv::Mat current_sprite = car_images[cars[i].selected_car_index];
			int sprite_w = current_sprite.cols;
			int sprite_h = current_sprite.rows;

			double time_param = (frame_count * 0.1) + cars[i].time_offset;
			double zigzag_vy = cars[i].vy_base + (std::sin(time_param) * 5.0);

			cars[i].x += cars[i].vx_base;
			cars[i].y += zigzag_vy;

			cars[i].x = std::fmod(cars[i].x + map_width, static_cast<double>(map_width));
			cars[i].y = std::fmod(cars[i].y + map_height, static_cast<double>(map_height));

			if (cars[i].x < 0) cars[i].x += map_width;
			if (cars[i].y < 0) cars[i].y += map_height;

			for (int py = 0; py < sprite_h; ++py) {
				for (int px = 0; px < sprite_w; ++px) {

					int draw_x = static_cast<int>(std::fmod(cars[i].x + px, static_cast<double>(map_width)));
					int draw_y = static_cast<int>(std::fmod(cars[i].y + py, static_cast<double>(map_height)));

					int screen_x = draw_x - visible_x;
					int screen_y = draw_y;

					if (screen_x < 0) screen_x += map_width;

					if (screen_x < map_width && screen_y < map_height) {
						if (current_sprite.channels() == 4) {
							cv::Vec4b pixel = current_sprite.at<cv::Vec4b>(py, px);
							uchar alpha = pixel[3];

							if (alpha > 10) {
								scroll_frame.at<cv::Vec3b>(screen_y, screen_x) = cv::Vec3b(pixel[0], pixel[1], pixel[2]);
							}
						}
						else {
							cv::Vec3b pixel = current_sprite.at<cv::Vec3b>(py, px);
							scroll_frame.at<cv::Vec3b>(screen_y, screen_x) = pixel;
						}
					}
				}
			}
		}

		cv::Mat process_frame = scroll_frame.clone();

		if (filter_mode == 1) {
		
			cv::bitwise_not(process_frame, process_frame);

		}

		cv::Mat noisy_frame = process_frame.clone();

		int num_noise_pixels = noise_intensity * 150; // Yoğunluk katsayısı

		for (int k = 0; k < num_noise_pixels; ++k) {
			int rand_x = std::rand() % noisy_frame.cols;
			int rand_y = std::rand() % noisy_frame.rows;

			uchar color_val = (std::rand() % 2 == 0) ? 255 : 0;
			noisy_frame.at<cv::Vec3b>(rand_y, rand_x) = cv::Vec3b(color_val, color_val, color_val);
		}

		cv::imshow("Simulation", noisy_frame);

		char key = (char)cv::waitKey(30);
		if (key == 27) { // ESC
			break;
		}
	}

	cv::destroyAllWindows();
	return 0;
}