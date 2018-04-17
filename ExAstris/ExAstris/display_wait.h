#pragma once
#include "init.h"
#include <atomic>
#include <random>

void master_wait(cimg_library::CImgDisplay&, cimg_library::CImgDisplay&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, Box_Field&);

void on_left_click(cimg_library::CImgDisplay&, cimg_library::CImg<unsigned char>);

void automatic(cimg_library::CImgDisplay&, cimg_library::CImgDisplay&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, Box_Field&, int, int, int, int);

void block_for_delay(int, std::atomic<bool>&);

double selection_deltaE(cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, std::pair<int, int>, std::pair<int, int>);

void image_diff(cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&);

void on_key_t(cimg_library::CImgDisplay&, cimg_library::CImgDisplay&, cimg_library::CImgDisplay&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, cimg_library::CImg<unsigned char>&, Box_Field&);

#pragma region classes

class Triangle
{

public:
	Triangle();
	Triangle(std::pair<int, int>, std::pair<int, int>, std::pair<int, int>, double, double[3], double);


	void print();


	std::vector<std::pair<int, int>> points;
	double opacity;
	double colour[3];
	double improvement;
	std::pair<int, int> top_left;
	std::pair<int, int> bottom_right;
};

bool operator< (const Triangle &t1, const Triangle &t2);
bool operator> (const Triangle &t1, const Triangle &t2);

class TrianglePool
{
public:
	TrianglePool();
	TrianglePool(Triangle, int);

	void seed(cimg_library::CImg<unsigned char>, cimg_library::CImg<unsigned char>);
	void breed(cimg_library::CImg<unsigned char>, cimg_library::CImg<unsigned char>);
	void mutate();
	void cull();
	void lifecycle();

	void print();

	std::vector<Triangle> pool;
	int capacity;

};

inline TrianglePool::TrianglePool()
{
	pool.emplace_back(Triangle());
	capacity = -1;
}

inline TrianglePool::TrianglePool(Triangle base_triangle, int capacity)
{
	pool.emplace_back(base_triangle);
	this->capacity = capacity;
}

inline void TrianglePool::seed(cimg_library::CImg<unsigned char> reference_crop, cimg_library::CImg<unsigned char> constructed_crop)
{
	while (pool.size() < capacity + 2)
	{
		double before = 0;
		double after = 1;
		do {
			//std::cout << "lol" << std::endl;
			std::vector<std::pair<int, int>> temp(3);
			temp[0] = { pool[0].points[0].first + randomInt(-3, 3), pool[0].points[0].second + randomInt(-3, 3) };
			temp[1] = { pool[0].points[1].first + randomInt(-3, 3), pool[0].points[1].second + randomInt(-3, 3) };
			temp[2] = { pool[0].points[2].first + randomInt(-3, 3), pool[0].points[2].second + randomInt(-3, 3) };
			double tempcolour[3] = { pool[0].colour[0] + randomInt(-2, 2), pool[0].colour[1] + randomInt(-2, 2), pool[0].colour[2] + randomInt(-2, 2) };

			cimg_library::CImg<unsigned char> temp_img(constructed_crop);

			cimg_library::CImg<unsigned char> orig_img(reference_crop);
			before = selection_deltaE(orig_img, constructed_crop, pool[0].top_left, pool[0].bottom_right);
			temp_img.draw_triangle(temp[0].first, temp[0].second, temp[1].first, temp[1].second, temp[2].first, temp[2].second, tempcolour, pool[0].opacity);

			std::pair<int, int> top_left = { std::min(std::min(temp[0].first, temp[1].first), temp[2].first), std::min(std::min(temp[0].second, temp[1].second), temp[2].second) };
			std::pair<int, int> bottom_right = { std::max(std::max(temp[0].first, temp[1].first), temp[2].first), std::max(std::max(temp[0].second, temp[1].second), temp[2].second) };

			after = selection_deltaE(orig_img, temp_img, top_left, bottom_right);
			if (before < after)
			{
				continue;
			}
			pool.emplace_back(*new Triangle(temp[0], temp[1], temp[2], pool[0].opacity, tempcolour, before - after));
		} while (before < after);
	}
}

inline void TrianglePool::breed(cimg_library::CImg<unsigned char> reference_crop, cimg_library::CImg<unsigned char> constructed_crop)
{
	for (int i = 0; i < 4; i++)
	{
		//candidates[i * 2]
		std::vector<std::pair<int, int>> temp(3);
		double tempcolour[3];
		double opacity;
		double improvement;
		for (int j = 0; j < 3; j++)
		{
			if (randomInt(0, 1))
			{
				if (randomInt(0, 1))
				{
					temp[j] = { pool[i * 2].points[j].first, pool[i * 2].points[j].second };
				}
				else
				{
					temp[j] = { pool[i * 2].points[j].first, pool[i * 2 + 1].points[j].second };
				}

			}
			else
			{
				if (randomInt(0, 1))
				{
					temp[j] = { pool[i * 2 + 1].points[j].first, pool[i * 2].points[j].second };
				}
				else
				{
					temp[j] = { pool[i * 2 + 1].points[j].first, pool[i * 2 + 1].points[j].second };
				}
			}

			if (randomInt(0, 1))
			{
				tempcolour[j] = pool[i * 2].colour[j];
			}
			else
			{
				tempcolour[j] = pool[i * 2 + 1].colour[j];
			}
		}

		if (randomInt(0, 1))
		{
			opacity = pool[i * 2].opacity;
		}
		else
		{
			opacity = pool[i * 2 + 1].opacity;
		}

		cimg_library::CImg<unsigned char> temp_img = constructed_crop;
		temp_img.draw_triangle(temp[0].first, temp[0].second, temp[1].first, temp[1].second, temp[2].first, temp[2].second, tempcolour, pool[0].opacity);

		std::pair<int, int> top_left = { std::min(std::min(temp[0].first, temp[1].first), temp[2].first), std::min(std::min(temp[0].second, temp[1].second), temp[2].second) };
		std::pair<int, int> bottom_right = { std::max(std::max(temp[0].first, temp[1].first), temp[2].first), std::max(std::max(temp[0].second, temp[1].second), temp[2].second) };
		double before = selection_deltaE(reference_crop, constructed_crop, top_left, bottom_right);
		double after = selection_deltaE(reference_crop, temp_img, top_left, bottom_right);
		improvement = before - after;
		pool.emplace_back(*new Triangle(temp[0], temp[1], temp[2], opacity, tempcolour, improvement));
	}
}

#pragma endregion classes