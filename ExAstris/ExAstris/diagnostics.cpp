#include "stdafx.h"
#include "diagnostics.h"




void diagnostics(cimg_library::CImgDisplay& reference_display, cimg_library::CImgDisplay& constructed_display, cimg_library::CImg<unsigned char>& reference_image_static, cimg_library::CImg<unsigned char>& constructed_image_static, cimg_library::CImg<unsigned char>& reference_image_dynamic, cimg_library::CImg<unsigned char>& constructed_image_dynamic, Box_Field& image_field)
{
	std::vector<std::vector<std::tuple<double, int, int>>> result = image_field.get_grid_rgb_stdev(reference_image_static);
	std::cout << std::setprecision(4);
	for (unsigned int i = 0; i < result[0].size(); i++)
	{
		/*for (int j = 0; j < 3; j++)
		{
			std::cout << std::get<0>(result[j][i]) << "\t";
		}

		std::cout << "\n";*/
		double avg = (std::get<0>(result[0][i]) + std::get<0>(result[1][i]) + std::get<0>(result[2][i])) / 3;
		double colour[3] = { avg, avg, avg };
		reference_image_dynamic.draw_rectangle(image_field.box_grid[std::get<1>(result[0][i])][std::get<2>(result[0][i])].get_top_left().first, image_field.box_grid[std::get<1>(result[0][i])][std::get<2>(result[0][i])].get_top_left().second, image_field.box_grid[std::get<1>(result[0][i])][std::get<2>(result[0][i])].get_bottom_right().first, image_field.box_grid[std::get<1>(result[0][i])][std::get<2>(result[0][i])].get_bottom_right().second, colour);
	}

	std::vector<std::tuple<double, int, int>> temp;

	for (unsigned int i = 0; i < result[0].size(); i++)
	{
		//temp.emplace_back(std::tuple<double, int, int>((sqrt(pow((2 * std::get<0>(result[0][i])), 2) + pow((4 * std::get<0>(result[1][i])), 2) + pow((3 * std::get<0>(result[2][i])), 2))), std::get<1>(result[0][i]), std::get<2>(result[0][i])));
		temp.emplace_back(std::tuple<double, int, int>((std::get<0>(result[0][i]) + std::get<0>(result[1][i]) + std::get<0>(result[2][i])) / 3, std::get<1>(result[0][i]), std::get<2>(result[0][i])));
	}
	double a[3] = { 255, 255, 255 };
	std::sort(temp.begin(), temp.end(), std::greater<>());

	int sum = 0;
	for (unsigned int i = 0; i < result[0].size(); i++)
	{
		if (std::get<0>(temp[i]) <= 10) {
			image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].set_calc_mills(500);
		} else
		{
			int lol = pow(std::get<0>(temp[i]) / 21.5, 2) * 500 + 400;
			image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].set_calc_mills(lol);
			sum += lol;
			//std::cout << "avg: " << std::get<0>(temp[i]) << " \tmills: " << pow((std::get<0>(temp[i]) / 60.0), 2) * 500 << '\n';
		}
		reference_image_dynamic.draw_text(image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().first, image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().second, std::to_string(image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_calc_mills()).c_str(), a);
	}
	std::cout << "sum of times: " << sum / 1000 << " seconds" << '\n';

	/*for (unsigned int i = 0; i < 24; i++)
	{
		reference_image_dynamic.draw_text(image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().first, image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().second, std::to_string(i).c_str(), a);
	}*/
	/*for (unsigned int i = 80; i < 100; i++)
	{
		reference_image_dynamic.draw_text(image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().first, image_field.box_grid[std::get<1>(temp[i])][std::get<2>(temp[i])].get_top_left().second, std::to_string(i).c_str(), a);
	}*/



}
