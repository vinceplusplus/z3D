
#include "../Core/stdint.h"

#include "../../FreeImage/Header.h"

#define FF_CSP_ONLY
#include "libav/ffImgfmt.h"

extern "C"
{
	#include "libav/avcodec.h"
	#include "libav/mem.h"
};

//#include "libav/avformat.h"

#include "AVIReader.h"

#include "AVIRenderer.h"

#include "libav/ffcodecs.h"
#include "libav/ffdshow_mediaguids.h"

extern "C"
{
	#include "libav/swscale.h"
};

#include <mmsystem.h>
#include "aviriff.h"

namespace z3D
{
	namespace Video
	{
		inline const FOURCC* getCodecFOURCCs(CodecID codecId)
		{
		 switch (codecId)
		  {
		   case CODEC_ID_MPEG4:
		   case CODEC_ID_SKAL:
		   case CODEC_ID_XVID4:
			{
			 static const FOURCC fccs[]={FOURCC_XVID,FOURCC_FFDS,FOURCC_FVFW,FOURCC_DX50,FOURCC_DIVX,FOURCC_MP4V,0};
			 return fccs;
			}
		   case CODEC_ID_MSMPEG4V3:
			{
			 static const FOURCC fccs[]={FOURCC_MP43,FOURCC_DIV3,0};
			 return fccs;
			}
		   case CODEC_ID_MSMPEG4V2:
			{
			 static const FOURCC fccs[]={FOURCC_MP42,FOURCC_DIV2,0};
			 return fccs;
			}
		   case CODEC_ID_MSMPEG4V1:
			{
			 static const FOURCC fccs[]={FOURCC_MP41,FOURCC_DIV1,0};
			 return fccs;
			}
		   case CODEC_ID_MPEG1VIDEO:
			{
			 static const FOURCC fccs[]={FOURCC_MPEG,FOURCC_MPG1,0};
			 return fccs;
			}
		   case CODEC_ID_MPEG2VIDEO:
			{
			 static const FOURCC fccs[]={FOURCC_MPEG,FOURCC_MPG2,0};
			 return fccs;
			}
		   case CODEC_ID_H263:
		   case CODEC_ID_H263P:
		   case CODEC_ID_H263I:
			{
			 static const FOURCC fccs[]={FOURCC_H263,FOURCC_S263,0};
			 return fccs;
			}
		   case CODEC_ID_H261:
			{
			 static const FOURCC fccs[]={FOURCC_H261,0};
			 return fccs;
			}
		   case CODEC_ID_WMV1:
			{
			 static const FOURCC fccs[]={FOURCC_WMV1,0};
			 return fccs;
			}
		   case CODEC_ID_WMV2:
			{
			 static const FOURCC fccs[]={FOURCC_WMV2,0};
			 return fccs;
			}
		   case CODEC_ID_WMV9_LIB:
			{
			 static const FOURCC fccs[]={FOURCC_WVC1,FOURCC_WMV3,FOURCC_WMV2,FOURCC_WMV1,FOURCC_MSS1,FOURCC_MSS2,FOURCC_WMVP,FOURCC_WVP2,0};
			 return fccs;
			}
		   case CODEC_ID_MJPEG:
			{
			 static const FOURCC fccs[]={FOURCC_MJPG,0};
			 return fccs;
			}
		   case CODEC_ID_LJPEG:
			{
			 static const FOURCC fccs[]={FOURCC_LJPG,0};
			 return fccs;
			}
		   case CODEC_ID_HUFFYUV:
			{
			 static const FOURCC fccs[]={FOURCC_HFYU,FOURCC_FFVH,0};
			 return fccs;
			}
		   case CODEC_ID_FFV1:
			{
			 static const FOURCC fccs[]={FOURCC_FFV1,0};
			 return fccs;
			}
		   case CODEC_ID_DVVIDEO:
			{
			 /* lowercase FourCC 'dvsd' for compatibility with MS DV decoder */
			 static const FOURCC fccs[]={mmioFOURCC('d','v','s','d'),FOURCC_DVSD,mmioFOURCC('d','v','2','5'),FOURCC_DV25,mmioFOURCC('d','v','5','0'),FOURCC_DV50,0};
			 return fccs;
			}
		   case CODEC_ID_THEORA:
		   case CODEC_ID_THEORA_LIB:
			{
			 static const FOURCC fccs[]={FOURCC_THEO,0};
			 return fccs;
			}
		   case CODEC_ID_H264:
		   case CODEC_ID_X264:
		   case CODEC_ID_X264_LOSSLESS:
			{
			 static const FOURCC fccs[]={FOURCC_H264,FOURCC_X264,FOURCC_AVC1,0};
			 return fccs;
			}
		   case CODEC_ID_SNOW:
			{
			 static const FOURCC fccs[]={FOURCC_SNOW,0};
			 return fccs;
			}
		   case CODEC_ID_FLV1:
			{
			 static const FOURCC fccs[]={FOURCC_FLV1,0};
			 return fccs;
			}
		   case CODEC_ID_CYUV:
			{
			 static const FOURCC fccs[]={FOURCC_CYUV,0};
			 return fccs;
			}
		   case CODEC_ID_CLJR:
			{
			 static const FOURCC fccs[]={FOURCC_CLJR,0};
			 return fccs;
			}
		   case CODEC_ID_Y800:
			{
			 static const FOURCC fccs[]={FOURCC_Y800,0};
			 return fccs;
			}
		   case CODEC_ID_444P:
			{
			 static const FOURCC fccs[]={FOURCC_444P,FOURCC_YV24,0};
			 return fccs;
			}
		   case CODEC_ID_422P:
			{
			 static const FOURCC fccs[]={FOURCC_422P,FOURCC_YV16,0};
			 return fccs;
			}
		   case CODEC_ID_411P:
			{
			 static const FOURCC fccs[]={FOURCC_411P,FOURCC_Y41B,0};
			 return fccs;
			}
		   case CODEC_ID_410P:
			{
			 static const FOURCC fccs[]={FOURCC_410P,0};
			 return fccs;
			}
		   default:
			{
			 static const FOURCC fccs[]={0,0};
			 return fccs;
			}
		  }
		}


		class AVIRendererInternal
		{
		public:
			AVCodecContext*				ctx;
			AVFrame*					src;
			AVFrame*					dst;
			fipImage					fip;
			SwsContext*					sws;
		public:
			AVIRendererInternal()
			{
				ctx=NULL;
				src=NULL;
				dst=NULL;
				sws=NULL;
			}
			~AVIRendererInternal()
			{
				if(dst) av_free(dst);
				if(src) av_free(src);
				if(ctx) avcodec_close(ctx);
				if(sws) sws_freeContext(sws);
			}
		};

		void AVIRenderer::__init()
		{
			if(_inited)
				return;

			avcodec_init();
			avcodec_register_all();

			// range of 200 is from bool lavc_codec(int x), is libavcodec supported codec? change it if the source of libavcodec updated
			for(int i=1;i<200;++i)
			{
				const FOURCC* cc=getCodecFOURCCs((CodecID)i);
				while(*cc)
				{
					_map_fcc_codecid[(long)(*cc)]=i;
					++cc;
				}
			}

			_inited=true;
		}

		void AVIRenderer::__shutdown()
		{
			av_free_static();
		}

		AVIRenderer::AVIRenderer()
		{
			_internal=new AVIRendererInternal();

			_last_frame=0xffffffff;
		}
		AVIRenderer::~AVIRenderer()
		{
			delete (AVIRendererInternal*)_internal;
		}
		AVIRenderer*				AVIRenderer::load(const wcs& filepath)
		{
			SPtr<AVIReader> reader = AVIReader::load(filepath);
			if(!reader)
				return NULL;
			AVIRenderer* renderer = load(reader);
			return renderer;
		}
		AVIRenderer*				AVIRenderer::load(const SPtr<AVIReader>& reader)
		{
			__init();

			if(!reader->video_header() || !reader->video_format())
				return NULL;

			if(!_map_fcc_codecid.count(((AVISTREAMHEADER*)reader->video_header())->fccHandler) && !_map_fcc_codecid.count(((BITMAPINFO*)reader->video_format())->bmiHeader.biCompression))
			{
				// handle in another way, custom rgba 32-bit raw or png
				if(((BITMAPINFO*)reader->video_format())->bmiHeader.biCompression != *(DWORD*)"PNG " && ((BITMAPINFO*)reader->video_format())->bmiHeader.biCompression != 0)
					return NULL;
				if(((BITMAPINFO*)reader->video_format())->bmiHeader.biBitCount != 32 && ((BITMAPINFO*)reader->video_format())->bmiHeader.biBitCount != 24 )
					return NULL;
				AVIRenderer* renderer = new AVIRenderer();
				renderer->_reader = reader;
				((AVIRendererInternal*)renderer->_internal)->fip.setSize(FIT_BITMAP, (WORD)((BITMAPINFO*)reader->video_format())->bmiHeader.biWidth, (WORD)((BITMAPINFO*)reader->video_format())->bmiHeader.biHeight, ((BITMAPINFO*)reader->video_format())->bmiHeader.biBitCount);
				return renderer;
			}
			unsigned long fcc = ((AVISTREAMHEADER*)reader->video_header())->fccHandler;
			if(!(_map_fcc_codecid.count(fcc) && avcodec_find_decoder((CodecID)_map_fcc_codecid[fcc])))
			{
				fcc = ((BITMAPINFO*)reader->video_format())->bmiHeader.biCompression;
				if(!(_map_fcc_codecid.count(fcc) && avcodec_find_decoder((CodecID)_map_fcc_codecid[fcc])))
					return NULL;
			}
			AVCodec* codec = avcodec_find_decoder((CodecID)_map_fcc_codecid[fcc]);
			AVCodecContext* ctx = avcodec_alloc_context();

			ctx->stream_codec_tag = ((AVISTREAMHEADER*)reader->video_header())->fccHandler;
			ctx->width = ((BITMAPINFO*)reader->video_format())->bmiHeader.biWidth;
			ctx->height = ((BITMAPINFO*)reader->video_format())->bmiHeader.biHeight;
			ctx->codec_tag = ((AVISTREAMHEADER*)reader->video_header())->fccHandler;
			// handle this carefully, is BITMAPINFOHEADER, not BITMAPINFO (which includes at least one RGBQUAD)
			ctx->extradata = (uint8_t*)((BITMAPINFOHEADER*)reader->video_format()+1);
			ctx->extradata_size = ((BITMAPINFO*)reader->video_format())->bmiHeader.biSize-sizeof(BITMAPINFOHEADER);

			if(avcodec_open(ctx,codec) != 0)
			{
				avcodec_close(ctx);
				return NULL;
			}

			AVIRenderer* renderer = new AVIRenderer();
			renderer->_reader = reader;
			AVIRendererInternal* intn = (AVIRendererInternal*)renderer->_internal;
			intn->ctx = ctx;
			intn->src = avcodec_alloc_frame();
			intn->dst = avcodec_alloc_frame();
			intn->fip.setSize(FIT_BITMAP, (WORD)ctx->width, (WORD)ctx->height, 24);
			avpicture_fill((AVPicture*)(intn->dst), intn->fip.accessPixels(), PIX_FMT_BGR24, ctx->width, ctx->height);
			intn->dst->linesize[0] = intn->fip.getScanWidth();

			SwsParams p;
			memset(&p, 0, sizeof(p));
			intn->sws = sws_getContext(ctx->width, ctx->height, csp_ffdshow2mplayer(csp_lavc2ffdshow(ctx->pix_fmt)), intn->fip.getWidth(), intn->fip.getHeight(), csp_ffdshow2mplayer(csp_lavc2ffdshow(PIX_FMT_BGR24)), &p, NULL, NULL, NULL);

			return renderer;
		}
		AVIRenderer*				AVIRenderer::loadNoCache(const wcs& filepath)
		{
			SPtr<AVIReader> reader = AVIReader::loadNoCache(filepath);
			if(!reader)
				return NULL;
			AVIRenderer* renderer = load(reader);
			return renderer;
		}

		bool						AVIRenderer::render(unsigned long frame)
		{
			frame=min(max(0,frame),_reader->frames()-1);

			if(frame==0xffffffff)
				return false;					// error?

			if(frame==_last_frame)
				return false;					// no need to update

			unsigned long keyframe=_reader->get_keyframe(frame);
			unsigned long f=keyframe;

			bool decode_twice = (_last_frame==0xffffffff && frame==keyframe);						// libavcodec workaround

			if(_last_frame != 0xffffffff && _last_frame >= keyframe && _last_frame < frame)
				f=_last_frame+1;

			bool updated=false;

			AVCodecContext* ctx=((AVIRendererInternal*)_internal)->ctx;
			AVFrame* src=((AVIRendererInternal*)_internal)->src;
			AVFrame* dst=((AVIRendererInternal*)_internal)->dst;

			do
			{
				_last_frame=f;
				if(_reader->get_frame_size(f)!=0)
				{
					if(ctx)
					{
						int got=0;
						if(avcodec_decode_video(ctx,src,&got,(uint8_t*)_reader->get_frame_data(f),_reader->get_frame_size(f)) == -1)
							return false;
						if(decode_twice)
						{
							if(avcodec_decode_video(ctx,src,&got,(uint8_t*)_reader->get_frame_data(f),_reader->get_frame_size(f)) == -1)
								return false;
							decode_twice=false;
						}
						if(got)
							updated=true;
					}
					else
					{
						fipImage& fip=((AVIRendererInternal*)_internal)->fip;
						if(!((BITMAPINFO*)_reader->video_format())->bmiHeader.biCompression)
							memcpy(fip.accessPixels(),_reader->get_frame_data(f),_reader->get_frame_size(f));
						else
						{
							fipMemoryIO memio((BYTE*)_reader->get_frame_data(f),_reader->get_frame_size(f));
							fipImage img;
							if(!img.loadFromMemory(memio))
								return false;
							fip=img;
						}
						updated=true;
					}
				}
				if(f==frame)
					break;
				++f;
			}while(true);

			if(!updated)
				return false;

			if(ctx)
			{
				fipImage& fip=((AVIRendererInternal*)_internal)->fip;

				// room to optimize not to flip the image
				sws_scale(((AVIRendererInternal*)_internal)->sws,src->data,src->linesize,0,ctx->height,dst->data,dst->linesize);

				fip.flipVertical();
			}

			return true;
		}

		bool						AVIRenderer::renderAt(REAL time)
		{
			return render((unsigned long)(time*_reader->frame_rate()));
		}

		const void*					AVIRenderer::get_fipImage() const
		{
			return &(((AVIRendererInternal*)_internal)->fip);
		}

		const AVIReader&			AVIRenderer::reader() const
		{
			return *_reader.get();
		}

		REAL						AVIRenderer::length() const
		{
			return (REAL)_reader->frames()/_reader->frame_rate();
		}

		unsigned long				AVIRenderer::dx() const
		{
			return ((BITMAPINFO*)_reader->video_format())->bmiHeader.biWidth;
		}

		unsigned long				AVIRenderer::dy() const
		{
			return ((BITMAPINFO*)_reader->video_format())->bmiHeader.biHeight;
		}

		unsigned long				AVIRenderer::last_frame() const
		{
			return _last_frame;
		}

		void						AVIRenderer::rescale_32bit(void* src,unsigned long src_dx,unsigned long src_dy,void* dst,unsigned long dst_dx,unsigned long dst_dy)
		{
			unsigned long src_scan=(src_dx<<2);
			unsigned long dst_scan=(dst_dx<<2);

			//unsigned long numerator_y=0;

			unsigned long* src_ptr_last=NULL;
			unsigned long* dst_ptr_last=NULL;

			unsigned long j;
			for(j=0;j<dst_dy;++j)
			{
				unsigned long* src_ptr=(unsigned long*)((char*)src+src_scan*(j*src_dy/dst_dy));
				unsigned long* dst_ptr=(unsigned long*)((char*)dst+dst_scan*j);

				if(src_ptr_last==src_ptr)
				{
					memcpy(dst_ptr,dst_ptr_last,dst_scan);
					continue;
				}
				src_ptr_last=src_ptr;
				dst_ptr_last=dst_ptr;

				unsigned long* dst_ptr_end=dst_ptr+dst_dx;

				unsigned long numerator_x=0;

				while(dst_ptr!=dst_ptr_end)
				{
					*dst_ptr=*src_ptr;
					++dst_ptr;

					numerator_x+=src_dx;
					while(numerator_x>=dst_dx)
					{
						numerator_x-=dst_dx;
						++src_ptr;
					}
				}
			}
		}

		void						AVIRenderer::rescale_24bit(void* src,unsigned long src_dx,unsigned long src_dy,void* dst,unsigned long dst_dx,unsigned long dst_dy)
		{
			typedef unsigned char TRI[3];

			unsigned long src_scan=((src_dx*3)+3)&0xfffffffc;
			unsigned long dst_scan=((dst_dx*3)+3)&0xfffffffc;

			//unsigned long numerator_y=0;

			TRI* src_ptr_last=NULL;
			TRI* dst_ptr_last=NULL;

			unsigned long j;
			for(j=0;j<dst_dy;++j)
			{
				TRI* src_ptr=(TRI*)((char*)src+src_scan*(j*src_dy/dst_dy));
				TRI* dst_ptr=(TRI*)((char*)dst+dst_scan*j);

				if(src_ptr_last==src_ptr)
				{
					memcpy(dst_ptr,dst_ptr_last,dst_scan);
					continue;
				}
				src_ptr_last=src_ptr;
				dst_ptr_last=dst_ptr;

				TRI* dst_ptr_end=dst_ptr+dst_dx;

				unsigned long numerator_x=0;

				while(dst_ptr!=dst_ptr_end)
				{
					(*dst_ptr)[0]=(*src_ptr)[0];
					(*dst_ptr)[1]=(*src_ptr)[1];
					(*dst_ptr)[2]=(*src_ptr)[2];

					++dst_ptr;

					numerator_x+=src_dx;
					while(numerator_x>=dst_dx)
					{
						numerator_x-=dst_dx;
						++src_ptr;
					}
				}
			}
		}
	};
};
