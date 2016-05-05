// ImagePacker2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ImagePacker2.h"
#define MAX_LOADSTRING 100

const int PADDING = 2;
const int NEAREST_FETCHES = 6;

class RECT2I
{
public:
	int			x;
	int			y;
	int			w;
	int			h;
public:
	RECT2I()
	{
		memset(this, 0, sizeof(*this));
	}
	RECT2I(int _w, int _h)
	{
		x = 0;
		y = 0;

		w = _w;
		h = _h;
	}
	RECT2I(int _x, int _y, int _w, int _h)
	{
		x = _x;
		y = _y;

		w = _w;
		h = _h;
	}
public:
	int			area() const
	{
		return w * h;
	}
};

vector<int> sortRects(const vector<RECT2I>& array_small)
{
	vector<int> index(array_small.size());
	int i, j;
	for(i = 0; i < (int)array_small.size(); ++i)
		index[i] = i;
	for(i = 0; i < (int)index.size() - 1; ++i)
		for(j = 0; j < (int)index.size() - 1 - i; ++j)
		{
			const RECT2I& rect0 = array_small[index[j]];
			const RECT2I& rect1 = array_small[index[j + 1]];
			int weight0, weight1;
			weight0 = max(rect0.w | (rect0.h << 16), rect0.h | (rect0.w << 16));
			weight1 = max(rect1.w | (rect1.h << 16), rect1.h | (rect1.w << 16));

			if(weight0 < weight1)
			{
				int t = index[j];
				index[j] = index[j + 1];
				index[j + 1] = t;
			}
		}
	return index;
}

bool canFit(const RECT2I& big, const vector<RECT2I>& array_small, vector<RECT2I>& output)
{
	output.resize(array_small.size());
	vector<RECT2I> array_spare;
	array_spare.push_back(big);
	vector<int> array_small_index = sortRects(array_small);

	int i, j;
	for(i = 0; i < (int)array_small_index.size(); ++i)
	{
		const RECT2I& current = array_small[array_small_index[i]];
		int min_remaining_area = 0x7fffffff;
		int min_index = -1;
		for(j = 0; j < (int)array_spare.size(); ++j)
		{
			RECT2I& spare = array_spare[j];
			if(spare.w >= current.w && spare.h >= current.h)
			{
				if(spare.area() - current.area() < min_remaining_area)
				{
					min_index = j;
					min_remaining_area = spare.area() - current.area();
				}
			}
		}
		if(min_index == -1)
			return false;
		RECT2I& spare = array_spare[min_index];
		RECT2I r0(0, 0), r1(0, 0);
		if(spare.w - current.w > spare.h - current.h)		// split horizontally
		{
			r0 = RECT2I(spare.x + current.w, spare.y, spare.w - current.w, spare.h);
			r1 = RECT2I(spare.x, spare.y + current.h, current.w, spare.h - current.h);
		}
		else												// split vertically
		{
			r0 = RECT2I(spare.x + current.w, spare.y, spare.w - current.w, current.h);
			r1 = RECT2I(spare.x, spare.y + current.h, spare.w, spare.h - current.h);
		}

		output[array_small_index[i]] = array_small[array_small_index[i]];
		output[array_small_index[i]].x = spare.x;
		output[array_small_index[i]].y = spare.y;

		array_spare.erase(array_spare.begin() + min_index);
		if(r0.area())
			array_spare.push_back(r0);
		if(r1.area())
			array_spare.push_back(r1);
	}
	return true;
}

fipImage clip(fipImage& image, RECT2I& clip_info)
{
	int x, y, x2, y2;
	x = 0x7fffffff;
	y = 0x7fffffff;
	x2 = 0x80000000;
	y2 = 0x80000000;

	fipImage clipped;
	
	int i, j;
	for(j = 0; j < (int)image.getHeight(); ++j)
	{
		unsigned long* pixel = (unsigned long*)image.getScanLine(image.getHeight() - 1 - j);
		for(i = 0; i < (int)image.getWidth(); ++i)
		{
			if((pixel[i] >> 24) > 0)
			{
				x = min(x, i);
				y = min(y, j);
				x2 = max(x2, i);
				y2 = max(y2, j);
			}
		}
	}
	image.copySubImage(clipped, x, y, x2 + 1, y2 + 1);
	clip_info.x = x;
	clip_info.y = y;
	clip_info.w = x2 - x + 1;
	clip_info.h = y2 - y + 1;
	if(clip_info.w < 0 || clip_info.h < 0)
	{
		clip_info.x = 0;
		clip_info.y = 0;
		clip_info.w = 0;
		clip_info.h = 0;
		syslog << L"empty image found";
	}

	return clipped;
}

vector<__int64> LOOKUP_NEAREST;

void init_nearest_lookup(int radius)
{
	multimap<REAL, __int64> nearest;

	int i, j;
	for(j = -radius; j <= radius; ++j)
		for(i = -radius; i <= radius; ++i)
		{
			if(!i && !j)
				continue;
			REAL r = SQRT((REAL)(i * i + j * j));
			nearest.insert(pair<REAL, __int64>(r, (((__int64)(unsigned long)j) << 32) | (unsigned long)i));
		}
	LOOKUP_NEAREST.clear();
	multimap<REAL, __int64>::iterator iter;
	for(iter = nearest.begin(); iter != nearest.end(); ++iter)
		LOOKUP_NEAREST.push_back(iter->second);
}

RGBQUAD getPixelColor(const fipImage& image, int x, int y)
{
	RGBQUAD q;
	q = *(RGBQUAD*)(((unsigned long*)image.accessPixels()) + ((image.getHeight() - 1 - y) * image.getWidth() + x));
	return q;
}

void setPixelColor(const fipImage& image, int x, int y, RGBQUAD q)
{
	*(RGBQUAD*)(((unsigned long*)image.accessPixels()) + ((image.getHeight() - 1 - y) * image.getWidth() + x)) = q;
}

bool nearest_colored_pixel(fipImage& image, int& x, int& y)
{
	int dx = (int)image.getWidth();
	int dy = (int)image.getHeight();

	int c = (int)LOOKUP_NEAREST.size();
	int i;
	for(i = 0; i < c; ++i)
	{
		int x2 = x + (int)LOOKUP_NEAREST[i];
		int y2 = y + (int)(LOOKUP_NEAREST[i] >> 32);

		if(x2 < 0 || x2 >= dx || y2 < 0 || y2 >= dy)
			continue;

		RGBQUAD q = getPixelColor(image, x2, y2);
		if(q.rgbReserved)
		{
			x = x2;
			y = y2;
			return true;
		}
	}
	return false;
}

bool nearest_color(fipImage& image, int x, int y, RGBQUAD& q)
{
	int dx = (int)image.getWidth();
	int dy = (int)image.getHeight();

	int count = 0;
	REAL weight_sum = 0.0f;
	REAL rgb[3] = {0.0f, 0.0f, 0.0f};

	int c = (int)LOOKUP_NEAREST.size();
	int i;
	for(i = 0; i < c; ++i)
	{
		int deltax, deltay;
		deltax = (int)LOOKUP_NEAREST[i];
		deltay = (int)(LOOKUP_NEAREST[i] >> 32);
		int x2 = x + deltax;
		int y2 = y + deltay;

		if(x2 < 0 || x2 >= dx || y2 < 0 || y2 >= dy)
			continue;

		RGBQUAD q2 = getPixelColor(image, x2, y2);
		if(q2.rgbReserved)
		{
			REAL weight = q2.rgbReserved / 255.0f * 1.0f / (REAL)(deltax * deltax + deltay * deltay);

			rgb[0] += q2.rgbRed * weight;
			rgb[1] += q2.rgbGreen * weight;
			rgb[2] += q2.rgbBlue * weight;

			weight_sum += weight;

			++count;
			if(count >= NEAREST_FETCHES)
			{
				q.rgbRed = (unsigned char)min(max((int)(rgb[0] / weight_sum), 0), 255);
				q.rgbGreen = (unsigned char)min(max((int)(rgb[1] / weight_sum), 0), 255);
				q.rgbBlue = (unsigned char)min(max((int)(rgb[2] / weight_sum), 0), 255);
				q.rgbReserved = 0;

				return true;
			}
		}
	}
	return false;
}

void fill_transparent(fipImage& image)
{
	fipImage image2(FIT_BITMAP, image.getWidth(), image.getHeight(), 32);

	image2 = image;

	int dx = (int)image.getWidth();
	int dy = (int)image.getHeight();

	int i, j;
	for(j = 0; j < dy; ++j)
		for(i = 0; i < dx; ++i)
		{
			RGBQUAD q = getPixelColor(image, i, j);

			if(!q.rgbReserved)
			{
				int x,y;
				x = i;
				y = j;
				if(nearest_colored_pixel(image2, x, y))
				{
					//q=getPixelColor(image2,x,y);
					//q.rgbReserved=0;
					nearest_color(image2, x, y, q);
					q.rgbReserved = 0;
					setPixelColor(image, i, j, q);
				}
			}
		}
}

void adjust_image(fipImage& image)
{
	fipImage image2(FIT_BITMAP, image.getWidth() + PADDING * 2, image.getHeight() + PADDING * 2, 32);

	memset(image2.accessPixels(), 0, (image.getWidth() + PADDING * 2) * (image.getHeight() + PADDING * 2) * 4);

	image2.pasteSubImage(image, PADDING, PADDING);

	image = image2;

	fill_transparent(image);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	init_nearest_lookup(5);

	wchar_t dir[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), dir, MAX_PATH);
	SetCurrentDirectory(wstring(dir).substr(0, wstring(dir).find_last_of('\\')).c_str());

	if(!wcslen(lpCmdLine))
	{
		MessageBox(NULL, L"Drop me a folder", L"Error", 0);
		return 0;
	}
	wcs input_dir = lpCmdLine;
	vector<wcs> array_filename = find_files_recursive(input_dir, L"*.png");
	vector<wcs> array_clipped_filename;
	vector<RECT2I> array_rect;
	vector<RECT2I> array_output_rect;
	vector<RECT2I> array_clip_info;
	vector<RECT2I> array_original_rect;
	int i, j;
	for(i = 0; i < (int)array_filename.size(); ++i)
	{
		fipImage image;
		image.loadU(array_filename[i].c_str());
		image.convertTo32Bits();

		array_original_rect.push_back(RECT2I((int)image.getWidth(), (int)image.getHeight()));
		RECT2I r;
		fipImage clipped = clip(image, r);

		adjust_image(clipped);

		wcs output_filename = FORMATW(L"%d.png", i);
		clipped.saveU(output_filename.c_str());
		array_clipped_filename.push_back(output_filename);
		array_clip_info.push_back(r);
	}
	for(i = 0; i < (int)array_clipped_filename.size(); ++i)
	{
		fipImage image;
		image.loadU(array_clipped_filename[i].c_str());
		array_rect.push_back(RECT2I((int)image.getWidth(), image.getHeight()));
	}

	void* prog_dlg = create_progress_dialog(L"ImagePacker2: calculating optimal rectangle packing");

	vector<RECT2I> array_output;
	int min_area = 0x7fffffff;
	int min_w = 0;
	int min_h = 0;
	const int DIMENSION_TRIES = 200;
	const int DIM_LOOKUP[4] = {0, 32, 64, 128};
	for(i = 1; i <= DIMENSION_TRIES; ++i)
	{
		for(j = 1; j <= DIMENSION_TRIES; ++j)
		{
			int dx = 256 * (i / 4) + DIM_LOOKUP[i % 4];
			int dy = 256 * (j / 4) + DIM_LOOKUP[j % 4];
			if(canFit(RECT2I(dx, dy), array_rect, array_output))
			{
				if(dx * dy < min_area)
				{
					min_area = dx * dy;
					min_w = dx;
					min_h = dy;
				}
			}
		}
		update_progress_dialog(prog_dlg, i * DIMENSION_TRIES * 100.0f / DIMENSION_TRIES / DIMENSION_TRIES);
	}
	if(min_area == 0x7fffffff)
	{
		MessageBox(NULL, L"cannot find the optimal rectangle packing", L"Error", 0);
		return 0;
	}
	syslog << FORMATW(L"min area: %d, min w: %d, min h: %d", min_area, min_w, min_h);

	canFit(RECT2I(min_w, min_h), array_rect, array_output);

	Config cfg(input_dir + L"/packed_cfg.txt");

	fipImage compound(FIT_BITMAP, min_w ,min_h, 32);
	for(i = 0;i < (int)array_clipped_filename.size(); ++i)
	{
		wcs prefix = FORMATW(L"image%d", i);
		cfg.setWString(prefix + L"", array_filename[i].substr(input_dir.length() + 1));
		cfg.setInt32(prefix + L"_offset_left", array_output[i].x + PADDING);
		cfg.setInt32(prefix + L"_offset_top", array_output[i].y + PADDING);
		cfg.setInt32(prefix + L"_width", array_original_rect[i].w);
		cfg.setInt32(prefix + L"_height", array_original_rect[i].h);
		cfg.setInt32(prefix + L"_clip_left", array_clip_info[i].x);
		cfg.setInt32(prefix + L"_clip_top", array_clip_info[i].y);
		cfg.setInt32(prefix + L"_clip_width", array_clip_info[i].w);
		cfg.setInt32(prefix + L"_clip_height", array_clip_info[i].h);

		if(!array_output[i].w || !array_output[i].h)
			syslog << FORMATW(L"warning: \"%s\" results in zero size", array_filename[i].c_str());

		fipImage image;
		image.loadU(array_clipped_filename[i].c_str());

		compound.pasteSubImage(image, array_output[i].x, array_output[i].y);
	}
	for(i = 0; i < (int)array_clipped_filename.size(); ++i)
		DeleteFile(array_clipped_filename[i].c_str());

	compound.saveU((input_dir + L"/packed.png").c_str());

	cfg.setInt32(L"total_images", (int)array_filename.size());

	return 0;
}

