#include "stdafx.h"
#include "init.h"
#include "diagnostics.h"
#include "display_wait.h"


int x_grid_number = 12;
int y_grid_number = 12;

std::random_device rd;
std::mt19937 gen(rd());

void init()
{
#pragma region initialize_displays
	cimg_library::CImg<unsigned char> reference_image_static("transistor.jpg");
	cimg_library::CImg<unsigned char> reference_image_dynamic(reference_image_static);

	double* averageColours = averageColour(reference_image_static);
	cimg_library::CImg<unsigned char> constructed_image_static(reference_image_static.width(), reference_image_static.height(), 1, 4);
	constructed_image_static.draw_rectangle(0, 0, 1920, 1080, averageColours, 1);
	cimg_library::CImg<unsigned char> constructed_image_dynamic(constructed_image_static);

	if (std::experimental::filesystem::exists("temp.svg"))
	{
		constructed_image_static = constructed_image_dynamic = "temp.svg";
		//constructed_image_static.blur(1, 1, 0, false, true);
		
	}

	for (int y = 0; y<int((constructed_image_static)._height); ++y) for (int x = 0; x<int((constructed_image_static)._width); ++x)
	{
		for (int colour = 0; colour <= 2; colour++)
		{
			constructed_image_dynamic(x, y, 0, colour) = abs(reference_image_static(x, y, 0, colour) - constructed_image_dynamic(x, y, 0, colour));
		}
	}

	cimg_library::CImgDisplay reference_display(reference_image_dynamic, "Reference", 0, false, false);
	cimg_library::CImgDisplay constructed_display(constructed_image_dynamic, "Constructed", 0, false, false);

	

#pragma endregion initialize_displays

	Box_Field image_field = *new Box_Field(constructed_image_dynamic, x_grid_number, y_grid_number);
	diagnostics(reference_display, constructed_display, reference_image_static, constructed_image_static, reference_image_dynamic, constructed_image_dynamic, image_field);
	reference_display.display(reference_image_dynamic);
	//constructed_display.display(constructed_image_dynamic);
	constructed_display.display(constructed_image_static);
	master_wait(reference_display, constructed_display, reference_image_static, constructed_image_static, reference_image_dynamic, constructed_image_dynamic, image_field);

}






template<typename F>
void timer(F function) {
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	function();
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << "Timed Microseconds elapsed: " << elapsed << std::endl;

	//return (double(tend) - tstart) / CLOCKS_PER_SEC;
}

int randomInt(int a, int b) {
	const std::uniform_int_distribution<> distribution(a, b);
	return distribution(gen);
}

#pragma region SD_helper_functions

	inline double single_sd(cimg_library::CImg<unsigned char>& image, std::pair<int, int> top_left, std::pair<int, int> bottom_right, int colour_channel)
{
	const int x_size = (bottom_right.first - top_left.first + 1);
	const int y_size = (bottom_right.second - top_left.second + 1);
	std::vector<double> colour_data(x_size * y_size);

	for (int x = 0; x < x_size; x++)
	{
		for (int y = 0; y < y_size; y++)
		{
			colour_data[x * y_size + y] = image(top_left.first + x, top_left.second + y, 0, colour_channel);
		}
	}

	const double colour_sum = std::accumulate(colour_data.begin(), colour_data.end(), 0.0);

	double colouravg = colour_sum / colour_data.size();

	std::transform(colour_data.begin(), colour_data.end(), colour_data.begin(), [colouravg](double i) {return (i - colouravg) * (i - colouravg); });

	const double coloursd = sqrt(std::accumulate(colour_data.begin(), colour_data.end(), 0.0) / (colour_data.size() - 1));

	//std::cout << coloursd << "\n";
	return coloursd;
}		

	inline std::vector<double> rgb_sd(cimg_library::CImg<unsigned char>& image, std::pair<int, int> top_left, std::pair<int, int> bottom_right)
{
	const int x_size = (bottom_right.first - top_left.first + 1);
	const int y_size = (bottom_right.second - top_left.second + 1);
	std::vector<std::vector<double>> colour_data(3, std::vector<double>(x_size * y_size));
	std::vector<double> result(3);


	for (int colour = 0; colour < 3; colour++) {
		for (int x = 0; x < x_size; x++)
		{
			for (int y = 0; y < y_size; y++)
			{
				colour_data[colour][x * y_size + y] = image(top_left.first + x, top_left.second + y, 0, colour);
			}
		}
	}
	for (int colour = 0; colour < 3; colour++) {
		const double colour_sum = std::accumulate(colour_data[colour].begin(), colour_data[colour].end(), 0.0);

		double colouravg = colour_sum / (x_size * y_size);

		std::transform(colour_data[colour].begin(), colour_data[colour].end(), colour_data[colour].begin(), [colouravg](double i) {return (i - colouravg) * (i - colouravg); });

		result[colour] = (sqrt(std::accumulate(colour_data[colour].begin(), colour_data[colour].end(), 0.0) / (colour_data[colour].size() - 1)));
	}

	return result;
}

#pragma endregion SD_helper_functions

#pragma region Box_functions

	Box::Box()
{
	this->top_left = { -1, -1 };
	this->bottom_right = { -1, -1 };
	this->calc_mills = -1;
}

	Box::Box(std::pair<int, int> top_left, std::pair<int, int> bottom_right)
{
	this->top_left = top_left;
	this->bottom_right = bottom_right;
	this->calc_mills = 750;
}

	std::pair<int, int> Box::get_top_left() const
{
	return this->top_left;
}

	std::pair<int, int> Box::get_bottom_right() const
{
	return this->bottom_right;
}

	void Box::set_top_left(std::pair<int, int> top_left)
{
	this->top_left = top_left;
}

	void Box::set_bottom_right(std::pair<int, int> bottom_right)
{
	this->bottom_right = bottom_right;
}

	int Box::get_calc_mills() const
	{
	return this->calc_mills;
}

	void Box::set_calc_mills(int time)
{
		this->calc_mills = time;
}


#pragma endregion Box_functions

#pragma region Box_Field_functions

	inline Box_Field::Box_Field()
{
	this->x_grid_number = -1;
	this->y_grid_number = -1;
}

	Box_Field::Box_Field(cimg_library::CImg<unsigned char>& image, int x_grid_number, int y_grid_number)
{
#pragma region point_grid_add
	this->x_grid_number = x_grid_number;
	this->y_grid_number = y_grid_number;

	const int x_grid_size = int(floor(image.width() / x_grid_number));
	const int y_grid_size = int(floor(image.height() / y_grid_number));
	this->point_grid.resize(y_grid_number + 1, std::vector<std::pair<int, int>>(x_grid_number + 1));

	for (int i = 0; i < x_grid_number; i++)
	{
		for (int j = 0; j < y_grid_number; j++)
		{
			this->point_grid[i][j] = { i * x_grid_size, j * y_grid_size };
		}
	}

	// handle last row
	for (int i = 0; i < x_grid_number; i++)
	{
		this->point_grid[i][y_grid_number] = { i * x_grid_size ,  image.height() - 1 };
	}

	// handle last column
	for (int j = 0; j < y_grid_number; j++)
	{
		this->point_grid[x_grid_number][j] = { image.width() - 1, j * y_grid_size };
	}

	// handle corner dot
	this->point_grid[x_grid_number][y_grid_number] = { image.width() - 1, image.height() - 1 };

	//color corner points for easy identification
	for (int j = 0; j < y_grid_number + 1; j++)
	{
		for (int i = 0; i < x_grid_number + 1; i++)
		{
			image(this->point_grid[i][j].first, this->point_grid[i][j].second, 0) = 255;
			image(this->point_grid[i][j].first, this->point_grid[i][j].second, 1) = 255;
			image(this->point_grid[i][j].first, this->point_grid[i][j].second, 2) = 255;
		}
	}
#pragma endregion point_grid_add

#pragma region box_grid_add
	this->box_grid.resize(x_grid_number, std::vector<Box>(y_grid_number));
	this->box_grid_bL.resize(x_grid_number, std::vector<Box>(y_grid_number - 1));
	this->box_grid_tR.resize(x_grid_number - 1, std::vector<Box>(y_grid_number));
	this->box_grid_bR.resize(x_grid_number - 1, std::vector<Box>(y_grid_number - 1));

	for (int i = 0; i < x_grid_number - 1; i++)
	{
		for (int j = 0; j < y_grid_number - 1; j++)
		{
			this->box_grid[i][j].set_top_left({ this->point_grid[i][j].first, this->point_grid[i][j].second });
			this->box_grid[i][j].set_bottom_right({ this->point_grid[i + 1][j + 1].first, this->point_grid[i + 1][j + 1].second });

			this->box_grid_bL[i][j].set_top_left({ this->point_grid[i][j].first, this->point_grid[i][j].second + y_grid_size / 2 });
			this->box_grid_bL[i][j].set_bottom_right({ this->point_grid[i + 1][j + 1].first, this->point_grid[i + 1][j + 1].second + y_grid_size / 2 });

			this->box_grid_tR[i][j].set_top_left({ this->point_grid[i][j].first + x_grid_size / 2 , this->point_grid[i][j].second });
			this->box_grid_tR[i][j].set_bottom_right({ this->point_grid[i + 1][j + 1].first + x_grid_size / 2 , this->point_grid[i + 1][j + 1].second });

			this->box_grid_bR[i][j].set_top_left({ this->point_grid[i][j].first + x_grid_size / 2, this->point_grid[i][j].second + y_grid_size / 2 });
			this->box_grid_bR[i][j].set_bottom_right({ this->point_grid[i + 1][j + 1].first + x_grid_size / 2, this->point_grid[i + 1][j + 1].second + y_grid_size / 2 });
		}
	}

	for (int i = 0; i < y_grid_number - 1; i++)
	{
		this->box_grid[i][y_grid_number - 1].set_top_left({ this->point_grid[i][y_grid_number - 1].first, this->point_grid[i][y_grid_number - 1].second });
		this->box_grid[i][y_grid_number - 1].set_bottom_right({ this->point_grid[i + 1][y_grid_number - 1 + 1].first, this->point_grid[i + 1][y_grid_number - 1 + 1].second });
		
		this->box_grid_tR[i][y_grid_number - 1].set_top_left({ this->point_grid[i][y_grid_number - 1].first + x_grid_size / 2, this->point_grid[i][y_grid_number - 1].second });
		this->box_grid_tR[i][y_grid_number - 1].set_bottom_right({ this->point_grid[i + 1][y_grid_number - 1 + 1].first + x_grid_size / 2, this->point_grid[i + 1][y_grid_number - 1 + 1].second });
	}

	for (int j = 0; j < x_grid_number - 1; j++)
	{
		this->box_grid[x_grid_number - 1][j].set_top_left({ this->point_grid[x_grid_number - 1][j].first, this->point_grid[x_grid_number - 1][j].second });
		this->box_grid[x_grid_number - 1][j].set_bottom_right({ this->point_grid[x_grid_number - 1 + 1][j + 1].first, this->point_grid[x_grid_number - 1 + 1][j + 1].second });

		this->box_grid_bL[x_grid_number - 1][j].set_top_left({ this->point_grid[x_grid_number - 1][j].first, this->point_grid[x_grid_number - 1][j].second + y_grid_size / 2 });
		this->box_grid_bL[x_grid_number - 1][j].set_bottom_right({ this->point_grid[x_grid_number - 1 + 1][j].first, this->point_grid[x_grid_number - 1 + 1][j + 1].second + y_grid_size / 2 });
	}

	this->box_grid[x_grid_number - 1][y_grid_number - 1].set_top_left({ this->point_grid[x_grid_number - 1][y_grid_number - 1].first, this->point_grid[x_grid_number - 1][y_grid_number - 1].second });
	this->box_grid[x_grid_number - 1][y_grid_number - 1].set_bottom_right({ this->point_grid[x_grid_number - 1 + 1][y_grid_number - 1 + 1].first, this->point_grid[x_grid_number - 1 + 1][y_grid_number - 1 + 1].second });


	/*for (int j = 0; j < this->box_grid_bR[0].size(); j++)
	{
		for (int i = 0; i < this->box_grid_bR.size(); i++)
		{
			std::cout << '(' << this->box_grid_bR[i][j].get_top_left().first << "," << this->box_grid_bR[i][j].get_top_left().second << '|' << this->box_grid_bR[i][j].get_bottom_right().first << "," << this->box_grid_bR[i][j].get_bottom_right().second << ") ";
		}
		std::cout << '\n';
	}


	for (int i = 0; i < x_grid_number; i++)
	{
		for (int j = 0; j < y_grid_number; j++)
		{
			std::cout << '(' << this->box_grid[i][j].get_top_left().first << "," << this->box_grid[i][j].get_top_left().second << '|' << this->box_grid[i][j].get_bottom_right().first << "," << this->box_grid[i][j].get_bottom_right().second << ") ";
		}
		std::cout << '\n';
	}*/

	/*for (int i = 0; i < x_grid_number; i++)
	{
		for (int j = 0; j < y_grid_number; j++)
		{
			this->box_grid[i][j].set_top_left({ this->point_grid[i][j].first, this->point_grid[i][j].second });
			this->box_grid[i][j].set_bottom_right({ this->point_grid[i + 1][j + 1].first, this->point_grid[i + 1][j + 1].second });
		}
	}*/

#pragma endregion box_grid_add
}

	std::vector<std::tuple<double, int, int>> Box_Field::get_grid_single_stdev(cimg_library::CImg<unsigned char>& image, int colour_channel)
{
	const int threads = 1;
	int xdivisions = -1;
	int ydivisions = -1;
	if (threads > this->x_grid_number && threads < this->x_grid_number * this->x_grid_number)
	{
		xdivisions = 1;
		ydivisions = (this->x_grid_number * this->x_grid_number) / threads;
	}
	else
	{
		xdivisions = this->x_grid_number / threads;
		ydivisions = this->y_grid_number;

	}
	std::vector<std::thread> threadpool(threads);
	std::vector<std::vector<std::tuple<double, int, int>>> return_values(threads, std::vector<std::tuple<double, int, int>>((this->x_grid_number * this->x_grid_number) / threads));
	std::vector<std::tuple<double, int, int>> result;

	for (int i = 0; i < this->x_grid_number / xdivisions; i++)
	{
		for (int j = 0; j < this->y_grid_number / ydivisions; j++) {
			threadpool[i * (this->y_grid_number / ydivisions) + j] = std::thread(&Box_Field::stdev_range_helper, this, image, i * xdivisions, (i + 1) * xdivisions, j * ydivisions, (j + 1) * ydivisions, colour_channel, std::ref(return_values[i * (this->y_grid_number / ydivisions) + j]));
		}
	}

	for (int i = 0; i < threads; i++)
	{
		threadpool[i].join();
	}

	for (int i = 0; i < threads; i++)
	{
		result.insert(result.end(), return_values[i].begin(), return_values[i].end());
	}
	return result;
}

	std::vector<std::vector<std::tuple<double, int, int>>> Box_Field::get_grid_rgb_stdev(cimg_library::CImg<unsigned char>& image)
{
	std::vector<std::vector<std::tuple<double, int, int>>> result(this->x_grid_number * this->x_grid_number * 3);
	for (int colour = 0; colour < 3; colour++)
	{
		result[colour] = get_grid_single_stdev(image, colour);
	}

	return result;
}

	void Box_Field::stdev_range_helper(cimg_library::CImg<unsigned char>& image, int startx, int stopx, int starty, int stopy, int colour_channel, std::vector<std::tuple<double, int, int>>& result)
{
	const int xsize = (stopx - startx);
	const int ysize = (stopy - starty);
	for (int x = 0; x < xsize; x++)
	{
		for (int y = 0; y < ysize; y++)
		{
			result[x * ysize + y] = { single_sd(image, this->box_grid[x + startx][y + starty].get_top_left(), this->box_grid[x + startx][y + starty].get_bottom_right(), colour_channel), x, y };
		}
	}
}

	std::pair<int, int> Box_Field::get_grid_numbers()
{
		return std::pair<int, int> {this->x_grid_number, this->y_grid_number};
}

#pragma endregion Box_Field_functions

#pragma region Triangle_functions
	Triangle::Triangle()
	{
		this->opacity = -1;
		this->colour[0] = 255;
		this->colour[1] = 255;
		this->colour[2] = 255;
		this->improvement = -1;
	}

	Triangle::Triangle(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, double opacity, double colour[3], double improvement)
	{
		this->points.emplace_back(p1);
		this->points.emplace_back(p2);
		this->points.emplace_back(p3);
		this->opacity = opacity;
		//this->colour = { colour[0], colour[0], colour[0] };
		this->colour[0] = colour[0];
		this->colour[1] = colour[1];
		this->colour[2] = colour[2];
		this->improvement = improvement;
	}

	void Triangle::print()
	{
		std::cout << "points: ";
		std::cout << "( " << this->points[0].first << "," << this->points[0].second << ")" << " " << "( " << this->points[1].first << "," << this->points[1].second << ")" << " " << "( " << this->points[2].first << "," << this->points[2].second << ")" << " " << '\n';
		std::cout << "opacity: ";
		std::cout << this->opacity << "\t";
		std::cout << "colour: ";
		std::cout << "R: " << this->colour[0] << " G: " << this->colour[1] << " B: " << this->colour[2] << '\t';
		std::cout << "improvement: ";
		std::cout << this->improvement << '\n';
	}


	bool operator<(const Triangle & t1, const Triangle & t2)
	{
		return t1.improvement < t2.improvement;
	}

	bool operator>(const Triangle & t1, const Triangle & t2)
	{
		return t1.improvement > t2.improvement;
	}
#pragma endregion Triangle_functions