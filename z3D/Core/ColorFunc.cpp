
#include "ColorFunc.h"
#include "Vector3.h"

namespace z3D
{
	namespace Core
	{
		Vec3		rgb2hsl(const Vec3& rgb)
		{
			Vec3 hsl;

			const REAL& r = rgb[0];
			const REAL& g = rgb[1];
			const REAL& b = rgb[2];

			REAL& h = hsl[0];
			REAL& s = hsl[1];
			REAL& l = hsl[2];

			REAL mini = min(r, min(g, b));
			REAL maxi = max(r, max(g, b));

			Z_ASSERT(mini >= 0.0f && maxi <= 1.0f);

			if(maxi == mini)
			{
				h = 0.0f;
				s = 0.0f;
				l = maxi;
			}
			else
			{
				if(maxi == r)
					h = (g - b) / (maxi - mini);
				else if(maxi == g)
					h =	(b - r) / (maxi - mini) + 2;
				else
					h = (r - g) / (maxi - mini) + 4;
				h /= 6;
				if(h < 0.0f)
					h += 1.0f;

				l = (maxi + mini) * 0.5f;

				if(l <= 0.5f)
					s = (maxi - mini) / (maxi + mini);
				else
					s = (maxi - mini) / (2 - (maxi + mini));
			}

			return hsl;
		}

		Vec3		hsl2rgb(const Vec3& hsl)
		{
			Vec3 rgb;

			const REAL& h = hsl[0];
			const REAL& s = hsl[1];
			const REAL& l = hsl[2];

			REAL& r = rgb[0];
			REAL& g = rgb[1];
			REAL& b = rgb[2];

			Z_ASSERT( h >= 0.0f && h <= 1.0f );

			REAL q = (l < 0.5f) ? (l * (1 + s)) : (l + s - (l * s));
			REAL p = 2 * l - q;

			const REAL ONE_THIRD = 1.0f / 3.0f;

			r = h + ONE_THIRD;
			g = h;
			b = h - ONE_THIRD;

			r = (r < 0) ? (r + 1.0f) : ((r > 1.0f) ? (r - 1.0f) : r);
			g = (g < 0) ? (g + 1.0f) : ((g > 1.0f) ? (g - 1.0f) : g);
			b = (b < 0) ? (b + 1.0f) : ((b > 1.0f) ? (b - 1.0f) : b);

			const REAL ONE_SIXTH = 1.0f / 6.0f;
			const REAL TWO_THIRD = 2.0f / 3.0f;

			if(r < ONE_SIXTH)		r = p + ((q - p) * 6 * r);
			else if(r < 0.5f)		r = q;
			else if(r < TWO_THIRD)	r = p + ((q - p) * 6 * (TWO_THIRD - r));
			else					r = q;

			if(g < ONE_SIXTH)		g = p + ((q - p) * 6 * g);
			else if(g < 0.5f)		g = q;
			else if(g < TWO_THIRD)	g = p + ((q - p) * 6 * (TWO_THIRD - g));
			else					g = q;

			if(b < ONE_SIXTH)		b = p + ((q - p) * 6 * b);
			else if(b < 0.5f)		b = q;
			else if(b < TWO_THIRD)	b = p + ((q - p) * 6 * (TWO_THIRD - b));
			else					b = q;

			return rgb;
		}

		Vec3		rgb2hsv(const Vec3& rgb)
		{
			Vec3 hsv;

			const REAL& r = rgb[0];
			const REAL& g = rgb[1];
			const REAL& b = rgb[2];

			REAL& h = hsv[0];
			REAL& s = hsv[1];
			REAL& v = hsv[2];

			REAL mini = min(r, min(g, b));
			REAL maxi = max(r, max(g, b));

			Z_ASSERT(mini >= 0.0f && maxi <= 1.0f);

			if(mini == maxi)
				h = 0.0f;
			else
			{
				if(maxi == r)
					h = (g - b) / (maxi - mini);
				else if(maxi == g)
					h =	(b - r) / (maxi - mini) + 2;
				else
					h = (r - g) / (maxi - mini) + 4;
				h /= 6;
				if(h < 0.0f)
					h += 1.0f;
			}

			if(maxi == 0.0f)
				s = 0.0f;
			else
				s = (maxi - mini) / maxi;

			v = maxi;

			return hsv;
		}

		Vec3		hsv2rgb(const Vec3& hsv)
		{
			Vec3 rgb;

			const REAL& h = hsv[0];
			const REAL& s = hsv[1];
			const REAL& v = hsv[2];

			REAL& r = rgb[0];
			REAL& g = rgb[1];
			REAL& b = rgb[2];

			Z_ASSERT( h >= 0.0f && h <= 1.0f );

			ptrdiff_t i = min(max((ptrdiff_t)0, (ptrdiff_t)(h * 6)), (ptrdiff_t)5);
			REAL f = h * 6 - i;
			REAL p = v * (1 - s);
			REAL q = v * (1 - f * s);
			REAL t = v * (1 - (1 - f) * s);

			switch(i)
			{
			case 0:		r = v;	g = t;	b = p;	break;
			case 1:		r = q;	g = v;	b = p;	break;
			case 2:		r = p;	g = v;	b = t;	break;
			case 3:		r = p;	g = q;	b = v;	break;
			case 4:		r = t;	g = p;	b = v;	break;
			case 5:		r = v;	g = p;	b = q;	break;
			default:	Z_ASSERT( false );
			}

			return rgb;
		}

		Vec3		hsl2hsv(const Vec3& hsl)
		{
			return rgb2hsv(hsl2rgb(hsl));
		}

		Vec3		hsv2hsl(const Vec3& hsv)
		{
			return rgb2hsl(hsv2rgb(hsv));
		}
	};
};

