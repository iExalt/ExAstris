#include "stdafx.h"
#include "display_wait.h"
#include <atomic>
//#include "CImg.h"

std::atomic <bool> active = true;
std::atomic <int> asd = 0;
int generation = 0;




void block_for_delay(int time, std::atomic<bool>& ring)
{
	cimg_library::cimg::sleep(time);
	ring = true;
}



double selection_deltaE(cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, std::pair<int, int> top_left, std::pair<int, int> bottom_right)
{
	double averageDeltaE = 0;
	const int area = abs(bottom_right.first + 1 - top_left.first) * abs(bottom_right.second + 1 - top_left.second);


	// sum colours and divide by area
	for (int x = top_left.first; x <= bottom_right.first; x++)
	{
		for (int y = top_left.second; y <= bottom_right.second; y++)
		{
			/*const double deltaR = pow((2 * abs(*reference_image_static.data(x, y, 0, 0) - *constructed_image_static.data(x, y, 0, 0))), 2);
			const double deltaG = pow((4 * abs(*reference_image_static.data(x, y, 0, 1) - *constructed_image_static.data(x, y, 0, 1))), 2);
			const double deltaB = pow((3 * abs(*reference_image_static.data(x, y, 0, 2) - *constructed_image_static.data(x, y, 0, 2))), 2);*/
			/*int leftR = reference_image_static(x, y, 0, 0);
			int rightR = constructed_image_static(x, y, 0, 0);
			int leftG = reference_image_static(x, y, 0, 1);
			int rightG = constructed_image_static(x, y, 0, 1);
			int leftB = reference_image_static(x, y, 0, 2);
			int rightB = constructed_image_static(x, y, 0, 2);

			double deltaR = (2 * abs(leftR - rightR)) * (2 * abs(leftR - rightR));
			double deltaG = (2 * abs(leftG - rightG)) * (2 * abs(leftG - rightG));
			double deltaB = (2 * abs(leftB - rightB)) * (2 * abs(leftB - rightB));*/
			
			
			double deltaR = pow((2 * abs(reference_image_static(x, y, 0, 0) - constructed_image_static(x, y, 0, 0))), 2);
			double deltaG = pow((4 * abs(reference_image_static(x, y, 0, 1) - constructed_image_static(x, y, 0, 1))), 2);
			double deltaB = pow((3 * abs(reference_image_static(x, y, 0, 2) - constructed_image_static(x, y, 0, 2))), 2);
			

			averageDeltaE += sqrt(deltaR + deltaG + deltaB);
		}
	}
	averageDeltaE /= area;

	return averageDeltaE;

}

inline void helper(cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, cimg_library::CImg<unsigned char>& constructed_image_dynamic, cimg_library::CImgDisplay& display1, cimg_library::CImg<unsigned char>& image1)
{
	image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
	display1.display(image1);
}

void image_diff(cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, cimg_library::CImg<unsigned char>& constructed_image_dynamic)
{
#pragma loop(hint_parallel(8))
	for (int colour_counter = 0; colour_counter <= 2; colour_counter++)
	{
		for (int y = 0; y<int((constructed_image_static)._height); ++y) for (int x = 0; x<int((constructed_image_static)._width); ++x)
		{
			constructed_image_dynamic(x, y, 0, colour_counter) = abs(reference_image_static(x, y, 0, colour_counter) - constructed_image_static(x, y, 0, colour_counter));
		}
	}
}

void master_wait(cimg_library::CImgDisplay& reference_display, cimg_library::CImgDisplay& constructed_display, cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, cimg_library::CImg<unsigned char>& reference_image_dynamic, cimg_library::CImg<unsigned char>& constructed_image_dynamic, Box_Field& image_field)
{


	cimg_library::CImgDisplay debug_display(cimg_library::CImg<unsigned char>(400, 400, 1, 3, 0), "Debug", 0, false, true);
	for (;;) {

#pragma region mouse_events
		//std::cout << "lol" << std::endl;
		//cimg_library::CImgDisplay::wait(reference_display, constructed_display, reference_cut, constructed_cut);
		cimg_library::CImgDisplay::wait(reference_display, constructed_display);
		//cimg_library::CImgDisplay::wait(constructed_display);
		//cimg_library::CImgDisplay::wait_all();

		if (reference_display.button() & 1) {
			on_left_click(reference_display, reference_image_dynamic);
		}
		else if (constructed_display.button() & 1)
		{
			on_left_click(constructed_display, constructed_image_dynamic);
		}
#pragma endregion mouse_events

#pragma region keyboard_events

		const char *const keycode1 = "I";
		const char *const keycode2 = "R";
		const char *const keycode3 = "T";
		const char *const keycode4 = "S";
		const char *const keycode5 = "O";
		const char *const keycode6 = "A";
		const char *const keycode7 = "E";
		const char *const keycode8 = "L";
		const char *const keycode9 = "P";
		const char *const keycode10 = "D";
		const char *const keycode11 = "B";
		if (reference_display.is_key(keycode1) || constructed_display.is_key(keycode1))
		{
			//constructed_displ
		}
		else if (reference_display.is_key(keycode2) || constructed_display.is_key(keycode2))
		{
			//automatic(reference_display, constructed_display, reference_image_static, constructed_image_static, reference_image_dynamic, constructed_image_dynamic, image_field, 0, 12, 0, 5);
			std::thread t1(automatic, std::ref(reference_display), std::ref(constructed_display), std::ref(reference_image_static), std::ref(constructed_image_static), std::ref(reference_image_dynamic), std::ref(constructed_image_dynamic), std::ref(image_field), 0, 12, 0, 12);
			//std::thread t2(automatic, std::ref(reference_display), std::ref(constructed_display), std::ref(reference_image_static), std::ref(constructed_image_static), std::ref(reference_image_dynamic), std::ref(constructed_image_dynamic), std::ref(image_field), 0, 12, 6, 12);
			/*std::thread t3(automatic, std::ref(reference_display), std::ref(constructed_display), std::ref(reference_image_static), std::ref(constructed_image_static), std::ref(reference_image_dynamic), std::ref(constructed_image_dynamic), std::ref(image_field), 0, 12, 6, 9);
			std::thread t4(automatic, std::ref(reference_display), std::ref(constructed_display), std::ref(reference_image_static), std::ref(constructed_image_static), std::ref(reference_image_dynamic), std::ref(constructed_image_dynamic), std::ref(image_field), 0, 12, 9, 12);*/

			t1.detach();
			//t2.detach();
			/*t3.detach();
			t4.detach();*/

		}
		else if (reference_display.is_key(keycode3) || constructed_display.is_key(keycode3) || debug_display.is_key(keycode3))
		{
			on_key_t(reference_display, constructed_display, debug_display, reference_image_static, constructed_image_static, reference_image_dynamic, constructed_image_dynamic, image_field);
		}
		else if (reference_display.is_key(keycode4) || constructed_display.is_key(keycode4))
		{
			active = !active;
			std::cout << "Active: " << std::boolalpha << active << "\n";
			//std::cout << "rand: " << randomInt(-5, 5) << '\n';
		}
		else if (reference_display.is_key(keycode5) || constructed_display.is_key(keycode5))
		{
			constructed_display.display(constructed_image_static);
		}
		else if (reference_display.is_key(keycode7) || constructed_display.is_key(keycode7))
		{
			constructed_image_static.save("temp.svg");
			constructed_image_static.save("temp.png");
			std::cout << "Saved!" << '\n';
		}
		else if (reference_display.is_key(keycode11) || constructed_display.is_key(keycode11))
		{
			constructed_image_static.blur(1, 1, 0, false, true);
			constructed_display.display(constructed_image_static);
		}

#pragma endregion keyboard_events
		reference_display.flush();
		constructed_display.flush();
		debug_display.flush();
	}
}


#pragma region events

void on_left_click(cimg_library::CImgDisplay& display, cimg_library::CImg<unsigned char> image)
{
	int mX = display.mouse_x();
	int mY = display.mouse_y();

	float pointr = *image.data(mX, mY, 0, 0);
	float pointg = *image.data(mX, mY, 0, 1);
	float pointb = *image.data(mX, mY, 0, 2);

	std::cout << "mX:" << mX << "\n";
	std::cout << "mY:" << mY << "\n";
	std::cout << "red: " << float(pointr) << "\n";
	std::cout << "green: " << float(pointg) << "\n";
	std::cout << "blue: " << float(pointb) << "\n";
	std::cout << "\n";

	display.set_button(1, false);
}

void automatic(cimg_library::CImgDisplay& reference_display, cimg_library::CImgDisplay& constructed_display, cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>&
	constructed_image_static, cimg_library::CImg<unsigned char>& reference_image_dynamic, cimg_library::CImg<unsigned char>& constructed_image_dynamic, Box_Field& image_field, int startx, int stopx, int starty, int stopy)
{
	// start at top left
	//for (int i = 0; i < image_field.get_grid_numbers().first; i++)
	//{
	//	for (int j = 0; j < image_field.get_grid_numbers().second; j++)
	//	{
	std::atomic <bool> timeup = false;
	for (int i = startx; i < stopx; i++)
	{
		for (int j = starty; j < stopy; j++)
		{
			if (!active)
			{
				std::cout << "Generation: " << generation << '\n';
				return;
			}

#pragma region tL
			{
				std::thread timerthread(block_for_delay, image_field.box_grid[i][j].get_calc_mills(), std::ref(timeup));
				timerthread.detach();
				std::pair<int, int> triangle_positions[3];
				int update_counter = 0;
				/*for (int triangle_index = 0; triangle_index <= 2; triangle_index++)
				{
					triangle_positions[triangle_index] = { -1, -1 };
				}*/
				const std::pair<int, int> x_value_range = std::minmax(image_field.box_grid[i][j].get_top_left().first, image_field.box_grid[i][j].get_bottom_right().first);
				const std::pair<int, int> y_value_range = std::minmax(image_field.box_grid[i][j].get_top_left().second, image_field.box_grid[i][j].get_bottom_right().second);


				while (!timeup)
				{
					//std::pair<int, int> candidate_point(randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second));
					/*for (int triangle_positions_index = 0; triangle_positions_index <= 2; triangle_positions_index++)
					{
						for (int candidate_point_index = 0; candidate_point_index <= 2; candidate_point_index++)
						{
							if (triangle_positions[candidate_point_index] == candidate_point)
							{
								candidate_point = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
								candidate_point_index = -1;
							}
						}
						triangle_positions[triangle_positions_index] = candidate_point;
					}*/
					for (int i = 0; i < 3; i++)
					{
						triangle_positions[i] = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
					}
					cimg_library::CImg<unsigned char> copy(constructed_image_static);
					std::pair<int, int> top_left = { std::min(std::min(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::min(std::min(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					std::pair<int, int> bottom_right = { std::max(std::max(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::max(std::max(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					double before = selection_deltaE(reference_image_static, copy, top_left, bottom_right);

					//double randomColour[] = { randomInt(0, 255), randomInt(0, 255), randomInt(0, 255) };
					double randomColour[3];

					double r = 0;
					double g = 0;
					double b = 0;
					const int area = abs(bottom_right.first + 1 - top_left.first) * abs(bottom_right.second + 1 - top_left.second);

					// sum colours and divide by area
					for (int x = top_left.first; x <= bottom_right.first; x++)
					{
						for (int y = top_left.second; y <= bottom_right.second; y++)
						{
							r += long(*reference_image_static.data(x, y, 0, 0));
							g += long(*reference_image_static.data(x, y, 0, 1));
							b += long(*reference_image_static.data(x, y, 0, 2));
						}
					}

					r /= area;
					g /= area;
					b /= area;
					randomColour[0] = abs(r + (randomInt(-7, 7)));
					randomColour[1] = abs(g + (randomInt(-7, 7)));
					randomColour[2] = abs(b + (randomInt(-7, 7)));

					double hardness = randomInt(20, 50) / 100.0;

					constructed_image_static.draw_triangle(triangle_positions[0].first, triangle_positions[0].second, triangle_positions[1].first, triangle_positions[1].second, triangle_positions[2].first, triangle_positions[2].second, randomColour, hardness);
					double after = selection_deltaE(reference_image_static, constructed_image_static, top_left, bottom_right);

					if (after + 1 > before)
					{
						constructed_image_static.assign(copy);
					}
					else
					{
						//std::cout << "Before: " << before << "\t" << "After: " << after << "\t" << "Total Change: " << before - after << " over " << (bottom_right.first + 1 - top_left.first) * (bottom_right.second + 1 - top_left.second) << " pixels" << "\n";
						update_counter++;
					}
					if (update_counter >= 120)
					{
						update_counter = 0;
						std::cout << "Update!" << '\n';
						image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
						constructed_display.display(constructed_image_static);
					}
					generation++;

				}
				timeup = false;
			}
#pragma endregion tL

#pragma region bL
			if (i < image_field.box_grid_bL.size() && j < image_field.box_grid_bL[0].size())
			{
				std::thread timerthread(block_for_delay, image_field.box_grid[i][j].get_calc_mills(), std::ref(timeup));
				timerthread.detach();
				std::pair<int, int> triangle_positions[3];
				int update_counter = 0;
				for (int triangle_index = 0; triangle_index <= 2; triangle_index++)
				{
					triangle_positions[triangle_index] = { -1, -1 };
				}
				const std::pair<int, int> x_value_range = std::minmax(image_field.box_grid_bL[i][j].get_top_left().first, image_field.box_grid_bL[i][j].get_bottom_right().first);
				const std::pair<int, int> y_value_range = std::minmax(image_field.box_grid_bL[i][j].get_top_left().second, image_field.box_grid_bL[i][j].get_bottom_right().second);


				while (!timeup)
				{
					std::pair<int, int> candidate_point(randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second));
					for (int triangle_positions_index = 0; triangle_positions_index <= 2; triangle_positions_index++)
					{
						for (int candidate_point_index = 0; candidate_point_index <= 2; candidate_point_index++)
						{
							if (triangle_positions[candidate_point_index] == candidate_point)
							{
								candidate_point = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
								candidate_point_index = -1;
							}
						}
						triangle_positions[triangle_positions_index] = candidate_point;
					}
					cimg_library::CImg<unsigned char> copy(constructed_image_static);
					std::pair<int, int> top_left = { std::min(std::min(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::min(std::min(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					std::pair<int, int> bottom_right = { std::max(std::max(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::max(std::max(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					double before = selection_deltaE(reference_image_static, copy, top_left, bottom_right);

					//double randomColour[] = { randomInt(0, 255), randomInt(0, 255), randomInt(0, 255) };
					double randomColour[3];

					double r = 0;
					double g = 0;
					double b = 0;
					const int area = abs(bottom_right.first + 1 - top_left.first) * abs(bottom_right.second + 1 - top_left.second);

					// sum colours and divide by area
					for (int x = top_left.first; x <= bottom_right.first; x++)
					{
						for (int y = top_left.second; y <= bottom_right.second; y++)
						{
							r += long(*reference_image_static.data(x, y, 0, 0));
							g += long(*reference_image_static.data(x, y, 0, 1));
							b += long(*reference_image_static.data(x, y, 0, 2));
						}
					}

					r /= area;
					g /= area;
					b /= area;
					randomColour[0] = abs(r + (randomInt(-7, 7)));
					randomColour[1] = abs(g + (randomInt(-7, 7)));
					randomColour[2] = abs(b + (randomInt(-7, 7)));

					double hardness = randomInt(20, 50) / 100.0;

					constructed_image_static.draw_triangle(triangle_positions[0].first, triangle_positions[0].second, triangle_positions[1].first, triangle_positions[1].second, triangle_positions[2].first, triangle_positions[2].second, randomColour, hardness);
					double after = selection_deltaE(reference_image_static, constructed_image_static, top_left, bottom_right);

					if (after + 3 > before)
					{

						constructed_image_static = copy;
					}
					else
					{
						//std::cout << "Before: " << before << "\t" << "After: " << after << "\t" << "Total Change: " << before - after << " over " << (bottom_right.first + 1 - top_left.first) * (bottom_right.second + 1 - top_left.second) << " pixels" << "\n";
						update_counter++;
					}
					if (update_counter >= 120)
					{
						update_counter = 0;
						std::cout << "Update!" << '\n';
						image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
						constructed_display.display(constructed_image_static);
					}
					generation++;

				}
				timeup = false;
			}
#pragma endregion bL



#pragma region tR
			if (i < image_field.box_grid_tR.size() && j < image_field.box_grid_tR[0].size())
			{
				std::thread timerthread(block_for_delay, image_field.box_grid[i][j].get_calc_mills(), std::ref(timeup));
				timerthread.detach();
				std::pair<int, int> triangle_positions[3];
				int update_counter = 0;
				for (int triangle_index = 0; triangle_index <= 2; triangle_index++)
				{
					triangle_positions[triangle_index] = { -1, -1 };
				}
				const std::pair<int, int> x_value_range = std::minmax(image_field.box_grid_tR[i][j].get_top_left().first, image_field.box_grid_tR[i][j].get_bottom_right().first);
				const std::pair<int, int> y_value_range = std::minmax(image_field.box_grid_tR[i][j].get_top_left().second, image_field.box_grid_tR[i][j].get_bottom_right().second);


				while (!timeup)
				{
					std::pair<int, int> candidate_point(randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second));
					for (int triangle_positions_index = 0; triangle_positions_index <= 2; triangle_positions_index++)
					{
						for (int candidate_point_index = 0; candidate_point_index <= 2; candidate_point_index++)
						{
							if (triangle_positions[candidate_point_index] == candidate_point)
							{
								candidate_point = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
								candidate_point_index = -1;
							}
						}
						triangle_positions[triangle_positions_index] = candidate_point;
					}
					cimg_library::CImg<unsigned char> copy(constructed_image_static);
					std::pair<int, int> top_left = { std::min(std::min(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::min(std::min(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					std::pair<int, int> bottom_right = { std::max(std::max(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::max(std::max(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					double before = selection_deltaE(reference_image_static, copy, top_left, bottom_right);

					//double randomColour[] = { randomInt(0, 255), randomInt(0, 255), randomInt(0, 255) };
					double randomColour[3];

					double r = 0;
					double g = 0;
					double b = 0;
					const int area = abs(bottom_right.first + 1 - top_left.first) * abs(bottom_right.second + 1 - top_left.second);

					// sum colours and divide by area
					for (int x = top_left.first; x <= bottom_right.first; x++)
					{
						for (int y = top_left.second; y <= bottom_right.second; y++)
						{
							r += long(*reference_image_static.data(x, y, 0, 0));
							g += long(*reference_image_static.data(x, y, 0, 1));
							b += long(*reference_image_static.data(x, y, 0, 2));
						}
					}

					r /= area;
					g /= area;
					b /= area;
					randomColour[0] = abs(r + (randomInt(-7, 7)));
					randomColour[1] = abs(g + (randomInt(-7, 7)));
					randomColour[2] = abs(b + (randomInt(-7, 7)));

					double hardness = randomInt(20, 50) / 100.0;

					constructed_image_static.draw_triangle(triangle_positions[0].first, triangle_positions[0].second, triangle_positions[1].first, triangle_positions[1].second, triangle_positions[2].first, triangle_positions[2].second, randomColour, hardness);
					double after = selection_deltaE(reference_image_static, constructed_image_static, top_left, bottom_right);

					if (after + 3 > before)
					{

						constructed_image_static = copy;
					}
					else
					{
						//std::cout << "Before: " << before << "\t" << "After: " << after << "\t" << "Total Change: " << before - after << " over " << (bottom_right.first + 1 - top_left.first) * (bottom_right.second + 1 - top_left.second) << " pixels" << "\n";
						update_counter++;
					}
					if (update_counter >= 120)
					{
						update_counter = 0;
						std::cout << "Update!" << '\n';
						image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
						constructed_display.display(constructed_image_static);
					}
					generation++;

				}
				timeup = false;
			}
#pragma endregion tR

#pragma region bR
			if (i < image_field.box_grid_bR.size() && j < image_field.box_grid_bR[0].size())
			{
				std::thread timerthread(block_for_delay, image_field.box_grid[i][j].get_calc_mills(), std::ref(timeup));
				timerthread.detach();
				std::pair<int, int> triangle_positions[3];
				int update_counter = 0;
				for (int triangle_index = 0; triangle_index <= 2; triangle_index++)
				{
					triangle_positions[triangle_index] = { -1, -1 };
				}
				const std::pair<int, int> x_value_range = std::minmax(image_field.box_grid_bR[i][j].get_top_left().first, image_field.box_grid_bR[i][j].get_bottom_right().first);
				const std::pair<int, int> y_value_range = std::minmax(image_field.box_grid_bR[i][j].get_top_left().second, image_field.box_grid_bR[i][j].get_bottom_right().second);


				while (!timeup)
				{
					std::pair<int, int> candidate_point(randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second));
					for (int triangle_positions_index = 0; triangle_positions_index <= 2; triangle_positions_index++)
					{
						for (int candidate_point_index = 0; candidate_point_index <= 2; candidate_point_index++)
						{
							if (triangle_positions[candidate_point_index] == candidate_point)
							{
								candidate_point = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
								candidate_point_index = -1;
							}
						}
						triangle_positions[triangle_positions_index] = candidate_point;
					}
					cimg_library::CImg<unsigned char> copy(constructed_image_static);
					std::pair<int, int> top_left = { std::min(std::min(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::min(std::min(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					std::pair<int, int> bottom_right = { std::max(std::max(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::max(std::max(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
					double before = selection_deltaE(reference_image_static, copy, top_left, bottom_right);

					//double randomColour[] = { randomInt(0, 255), randomInt(0, 255), randomInt(0, 255) };
					double randomColour[3];

					double r = 0;
					double g = 0;
					double b = 0;
					const int area = abs(bottom_right.first + 1 - top_left.first) * abs(bottom_right.second + 1 - top_left.second);

					// sum colours and divide by area
					for (int x = top_left.first; x <= bottom_right.first; x++)
					{
						for (int y = top_left.second; y <= bottom_right.second; y++)
						{
							r += long(*reference_image_static.data(x, y, 0, 0));
							g += long(*reference_image_static.data(x, y, 0, 1));
							b += long(*reference_image_static.data(x, y, 0, 2));
						}
					}

					r /= area;
					g /= area;
					b /= area;
					randomColour[0] = abs(r + (randomInt(-7, 7)));
					randomColour[1] = abs(g + (randomInt(-7, 7)));
					randomColour[2] = abs(b + (randomInt(-7, 7)));

					double hardness = randomInt(20, 50) / 100.0;

					constructed_image_static.draw_triangle(triangle_positions[0].first, triangle_positions[0].second, triangle_positions[1].first, triangle_positions[1].second, triangle_positions[2].first, triangle_positions[2].second, randomColour, hardness);
					double after = selection_deltaE(reference_image_static, constructed_image_static, top_left, bottom_right);

					if (after + 3 > before)
					{

						constructed_image_static = copy;
					}
					else
					{
						//std::cout << "Before: " << before << "\t" << "After: " << after << "\t" << "Total Change: " << before - after << " over " << (bottom_right.first + 1 - top_left.first) * (bottom_right.second + 1 - top_left.second) << " pixels" << "\n";
						update_counter++;
					}
					if (update_counter >= 120)
					{
						update_counter = 0;
						std::cout << "Update!" << '\n';
						image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
						constructed_display.display(constructed_image_static);
					}
					generation++;

				}
				timeup = false;
			}
#pragma endregion bR



			std::thread temp(helper, std::ref(reference_image_static), std::ref(constructed_image_static), std::ref(constructed_image_dynamic), std::ref(constructed_display), std::ref(constructed_image_static));
			temp.detach();
		}

		//std::cout << "Update!" << '\n';

		/*image_diff(reference_image_static, constructed_image_static, constructed_image_dynamic);
		constructed_display.display(constructed_image_static);*/

		std::cout << "Generations: " << generation << '\n';
	}
}

void on_key_t(cimg_library::CImgDisplay& reference_display, cimg_library::CImgDisplay& constructed_display, cimg_library::CImgDisplay& debug_display, cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, cimg_library::CImg<unsigned char>& reference_image_dynamic, cimg_library::CImg<unsigned char>& constructed_image_dynamic, Box_Field& image_field)
{
#pragma region initial_triangle
	std::pair<int, int> crop_tL = image_field.box_grid[6][6].get_top_left();
	std::pair<int, int> crop_bR = image_field.box_grid[6][6].get_bottom_right();

	if (crop_tL.first > 25 && crop_bR.first < reference_image_static.width() - 1 - 25)
	{
		crop_tL.first -= 25;
		crop_bR.first += 25;
	}
	else if (crop_tL.first < 25)
	{
		crop_bR.first += 25 - crop_tL.first;
		crop_tL.first = 0;
	}
	else
	{
		crop_tL.first -= 25 - (reference_image_static.width() - 1) - crop_bR.first;
	}
	if (crop_tL.second > 25 && crop_bR.second < reference_image_static.height() - 1 - 25)
	{
		crop_tL.second -= 25;
		crop_bR.second += 25;
	}
	else if (crop_tL.second < 25)
	{
		crop_bR.second += 25 - crop_tL.second;
		crop_tL.second = 0;
	}
	else
	{
		crop_tL.second -= 25 - (reference_image_static.height() - 1) - crop_bR.second;
	}

	cimg_library::CImg<unsigned char> reference_crop(reference_image_static.get_crop(crop_tL.first, crop_tL.second, crop_bR.first, crop_bR.second));
	cimg_library::CImg<unsigned char> constructed_crop(constructed_image_static.get_crop(crop_tL.first, crop_tL.second, crop_bR.first, crop_bR.second));

	//double z[3] = { 113, 53, 68 };
	//Triangle newTriangle({ 37, 30 }, {117, 29}, { 112, 131 }, 0.3, z, -1);

	std::pair<int, int> triangle_positions[3];
	const std::pair<int, int> x_value_range = { 0, crop_bR.first - crop_tL.first };
	const std::pair<int, int> y_value_range = { 0, crop_bR.second - crop_tL.second };

	for (int i = 0; i < 3; i++)
	{
		triangle_positions[i] = { randomInt(x_value_range.first, x_value_range.second), randomInt(y_value_range.first, y_value_range.second) };
	}

	//cropped.draw_triangle(20, 20, 20, 20, 20, 20, col);
	std::pair<int, int> base_top_left = { std::min(std::min(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::min(std::min(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };
	std::pair<int, int> base_bottom_right = { std::max(std::max(triangle_positions[0].first, triangle_positions[1].first), triangle_positions[2].first), std::max(std::max(triangle_positions[0].second, triangle_positions[1].second), triangle_positions[2].second) };

	double r = 0;
	double g = 0;
	double b = 0;
	const int area = abs(base_bottom_right.first + 1 - base_top_left.first) * abs(base_bottom_right.second + 1 - base_top_left.second);

	// sum colours and divide by area
	for (int x = base_top_left.first; x <= base_bottom_right.first; x++)
	{
		for (int y = base_top_left.second; y <= base_bottom_right.second; y++)
		{
			r += long(*reference_crop.data(x, y, 0, 0));
			g += long(*reference_crop.data(x, y, 0, 1));
			b += long(*reference_crop.data(x, y, 0, 2));
		}
	}

	r /= area;
	g /= area;
	b /= area;

	double col[3] = { r, g, b };
	double opacity = randomInt(20, 50) / 100.0;

	Triangle base_triangle(triangle_positions[0], triangle_positions[1], triangle_positions[2], 0.3, col, opacity);

#pragma endregion initial_triangle
	std::vector<Triangle> candidates;
	while (candidates.size() < 8)
	{
		double before = 0;
		double after = 1;
		do {
			//std::cout << "lol" << std::endl;
			std::vector<std::pair<int, int>> temp(3);
			temp[0] = { base_triangle.points[0].first + randomInt(-3, 3), base_triangle.points[0].second + randomInt(-3, 3) };
			temp[1] = { base_triangle.points[1].first + randomInt(-3, 3), base_triangle.points[1].second + randomInt(-3, 3) };
			temp[2] = { base_triangle.points[2].first + randomInt(-3, 3), base_triangle.points[2].second + randomInt(-3, 3) };
			double tempcolour[3] = { base_triangle.colour[0] + randomInt(-2, 2), base_triangle.colour[1] + randomInt(-2, 2), base_triangle.colour[2] + randomInt(-2, 2) };

			cimg_library::CImg<unsigned char> temp_img(constructed_crop);

			cimg_library::CImg<unsigned char> orig_img(reference_crop);
			before = selection_deltaE(orig_img, constructed_crop, base_top_left, base_bottom_right);
			temp_img.draw_triangle(temp[0].first, temp[0].second, temp[1].first, temp[1].second, temp[2].first, temp[2].second, tempcolour, base_triangle.opacity);

			std::pair<int, int> top_left = { std::min(std::min(temp[0].first, temp[1].first), temp[2].first), std::min(std::min(temp[0].second, temp[1].second), temp[2].second) };
			std::pair<int, int> bottom_right = { std::max(std::max(temp[0].first, temp[1].first), temp[2].first), std::max(std::max(temp[0].second, temp[1].second), temp[2].second) };
			
			after = selection_deltaE(orig_img, temp_img, top_left, bottom_right);
			if (before < after)
			{
				continue;
			}
			candidates.emplace_back(*new Triangle(temp[0], temp[1], temp[2], base_triangle.opacity, tempcolour, before - after));
		} while (before < after);
	}

	std::sort(candidates.begin(), candidates.end(), std::greater<Triangle>());
	for (auto & val : candidates)
	{
		val.print();
	}


	


	std::sort(candidates.begin(), candidates.end(), std::greater<Triangle>());
	for (auto & val : candidates)
	{
		val.print();
	}





	cimg_library::CImg<unsigned char> temp_img = constructed_crop;
	std::cout << constructed_crop.width() << ", " << constructed_crop.height() << '\n';
	temp_img.draw_triangle(candidates[0].points[0].first, candidates[0].points[0].second, candidates[0].points[1].first, candidates[0].points[1].second, candidates[0].points[2].first, candidates[0].points[2].second, candidates[0].colour, base_triangle.opacity);
	//constructed_display.display(constructed_image_static);

	debug_display.display(temp_img);
	debug_display.resize(temp_img, true);
	debug_display.show();
	constructed_image_static.draw_triangle(candidates[0].points[0].first + crop_tL.first, candidates[0].points[0].second + crop_tL.second, candidates[0].points[1].first + crop_tL.first, candidates[0].points[1].second + crop_tL.second, candidates[0].points[2].first + crop_tL.first, candidates[0].points[2].second + crop_tL.second, candidates[0].colour, base_triangle.opacity);
	



}

#pragma endregion events