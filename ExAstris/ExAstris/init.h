#pragma once

void init();

inline double* averageColour(cimg_library::CImg<unsigned char> image)
{
	double r = 0;
	double g = 0;
	double b = 0;
	double averageDeltaE = 0;
	const int area = image.width() * image.height();
	double* average = new double[3];

	// sum colours and divide by area
	cimg_forXY(image, x, y)
	{
		r += long(*image.data(x, y, 0, 0));
		g += long(*image.data(x, y, 0, 1));
		b += long(*image.data(x, y, 0, 2));
	}

	average[0] = r / area;
	average[1] = g / area;
	average[2] = b / area;

	cimg_forXY(image, x, y)
	{
		/*unsigned char pointr = *image.data(x, y, 0, 0);
		unsigned char pointg = *image.data(x, y, 0, 1);
		unsigned char pointb = *image.data(x, y, 0, 2);*/

		double deltaR = pow((2 * abs(*image.data(x, y, 0, 0) - r)), 2);
		double deltaG = pow((4 * abs(*image.data(x, y, 0, 1) - g)), 2);
		double deltaB = pow((3 * abs(*image.data(x, y, 0, 2) - b)), 2);

		averageDeltaE += sqrt(deltaR + deltaG + deltaB);
	}
	return average;
}

int randomInt(int, int);

#pragma region classes
class Box
{
public:
	Box();
	Box(std::pair<int, int>, std::pair<int, int>);

	std::pair<int, int> get_top_left() const;
	void set_top_left(std::pair<int, int>);
	std::pair<int, int> get_bottom_right() const;
	void set_bottom_right(std::pair<int, int>);
	int get_calc_mills() const;
	void set_calc_mills(int);

	//std::array<double, 3> get_stdev(cimg_library::CImg<unsigned char>) const;
	//double get_stdev_single(cimg_library::CImg<unsigned char>, int) const;

private:
	std::pair<int, int> top_left;
	std::pair<int, int> bottom_right;
	int calc_mills;
};

class Box_Grid
{
	std::vector<std::vector<Box>> grid;
};

class Box_Field
{
public:
	Box_Field();
	Box_Field(cimg_library::CImg<unsigned char>&, int, int);

	std::vector<std::tuple<double, int, int>> get_grid_single_stdev(cimg_library::CImg<unsigned char>&, int);
	std::vector<std::vector<std::tuple<double, int, int>>> get_grid_rgb_stdev(cimg_library::CImg<unsigned char>&);
	void stdev_range_helper(cimg_library::CImg<unsigned char>&, int, int, int, int, int, std::vector<std::tuple<double, int, int>>&);
	std::vector<std::vector<Box>> box_grid;
	std::vector<std::vector<Box>> box_grid_bL;
	std::vector<std::vector<Box>> box_grid_tR;
	std::vector<std::vector<Box>> box_grid_bR;

	std::pair<int, int> get_grid_numbers();

private:
	int x_grid_number;
	int y_grid_number;
	std::vector<std::vector<std::pair<int, int>>> point_grid;
};

#pragma endregion classes

template<typename F>
void timer(F);