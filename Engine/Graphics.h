/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "Sprite.h"
#include "Rect.h"
#include <memory>
#include "BlurProcessor.h"
//#include "Effects.h"
#include "assert.h"
class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	void EndFrame();
	void BeginFrame();
	void PutPixel( int x,int y,int r,int g,int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel(int index, Color c);
	void PutPixel( int x,int y,Color c );
	template <typename E>
	void DrawSprite(int x, int y, Sprite& s, RectI SpritePortion, RectI& clip , E effect)
	{
		assert(WithinScreen(SpritePortion));

		if (clip.left > x)
		{
			SpritePortion.left += std::abs(clip.left - x);
			SpritePortion.width -= std::abs(clip.left - x);
			x = clip.left;
		}
		if (clip.top > y)
		{
			SpritePortion.top += clip.top - y;
			SpritePortion.height -= clip.top - y;
			y = clip.top;
		}

		if (clip.left + clip.width < x + SpritePortion.width)
		{
			SpritePortion.width = std::abs(clip.left + clip.width - x);
		}
		if (clip.top + clip.height < y + SpritePortion.height)
		{
			SpritePortion.height = std::abs(clip.top + clip.height - y);
		}

		const int StopY = SpritePortion.top + SpritePortion.height;
		const int StopX = SpritePortion.left + SpritePortion.width;

		for (int sy = SpritePortion.top; sy < StopY; sy++)
		{
			for (int sx = SpritePortion.left; sx < StopX; sx++)
			{
				Color c = s.GetPixel(sx, sy);
				effect(std::move(c), *this, x + sx - SpritePortion.left, y + sy - SpritePortion.top);
			}
		}
	}
	static RectI GetScreenRect();

	template <typename T>
	void DrawRect(Rect_<T>& rect, Color c)
	{
		for (int y = int(rect.top); y < int(rect.top + rect.height); y++)
		{
			for (int x = rect.left; x < int(rect.left + rect.width); x++)
				PutPixel(x, y, c);
		}
	}

	template<typename T>
	void DrawRect_Transparent(Rect_<T>& rect, Color c, Sprite& sprite, int transparency)
	{
		float tFactor = float(transparency / 100.0f);
		for (int y = rect.top; y < rect.top + rect.height; y++)
		{
			for (int x = rect.left; x < rect.left + rect.width; x++)
			{
				Color sColor = sprite.GetPixel(x, y);
				int r = (c.GetR() + sColor.GetR()) * tFactor, b = (c.GetB() + sColor.GetB()) * tFactor,
					g = (c.GetG() + sColor.GetG()) * tFactor;
				PutPixel(int(x), int(y), r, g, b);
			}
		}
	}

	template<typename T>
	void DrawRect_Transparent(Rect_<T>& rect, Color c, int transparency)
	{
		float tFactor = float(transparency / 100.0f);
		for (int y = rect.top; y < rect.top + rect.height; y++)
		{
			for (int x = rect.left; x < rect.left + rect.width; x++)
			{
				Color sColor = pSysBuffer[y * ScreenWidth + x];
				int r = (c.GetR() + sColor.GetR()) * tFactor, b = (c.GetB() + sColor.GetB()) * tFactor,
					g = (c.GetG() + sColor.GetG()) * tFactor;
				PutPixel(int(x), int(y), r, g, b);
			}
		}
	}

	template<typename T>
	static bool WithinScreen(Rect_<T>& rect){
		return (rect.left >= 0 && rect.left + rect.width <= Graphics::ScreenWidth &&
			rect.top >= 0 && rect.top + rect.height <= Graphics::ScreenHeight);

	}
	template <typename T>
	void DrawRect_Border(Rect_<T>& rect, Color c)
	{
		for (int y = int(rect.top); y < int(rect.top + rect.height); y += 1)
		{
			for (int x = int(rect.left); x < int(rect.left + rect.width); x += 1)
			{
				if (y == rect.top)
				{
					PutPixel(x, y, c);
				}
				else if (x == rect.left || x == rect.left + rect.width - 1)
				{
					PutPixel(x, y, c);
				}
				else if (y == rect.bottom() - 1)
				{
					PutPixel(x, y, c);
				}
			}
		}
	}
	static bool WithinScreen(Vec2I& pos)
	{
		return pos.x >= 0 && pos.x <= ScreenWidth && pos.y >= 0 && pos.y <= ScreenHeight;
	}

	template<typename T>
	void DrawAngledSprite(int x, int y, Sprite& s, Rect_<T>& SpritePortion, Vec2I& pos)
	{
		const short StartX = SpritePortion.top - SpritePortion.width / 2;
		const short StartY = SpritePortion.top - SpritePortion.height / 2;

		const short StopY = SpritePortion.top + SpritePortion.height / 2;
		const short StopX = SpritePortion.left + SpritePortion.width / 2;

		RectI SRect = { SpritePortion.width , SpritePortion.height , Vec2I{x , y } };
		Vec2I SCenter = SRect.GetCenter();

		float hyphotenuse = SCenter.GetLenght(pos);
		float sin = SCenter.GetSin(pos , hyphotenuse);
		float cos = SCenter.GetCos(pos , hyphotenuse);

		short hWidth = SpritePortion.width / 2;
		short hHeight = SpritePortion.height / 2;
		for (short sy = StartX; sy < StopY; sy++)
		{
			for (short sx = StartY; sx < StopX; sx++)
			{
				Color c = s.GetPixel(sx + hWidth, sy + hHeight);

				Vec2I new_pos = Vec2I( SCenter.x + sx * cos - sy * sin ,
								  SCenter.y + sx * sin + sy * cos );

				if (WithinScreen(new_pos))
				{
					PutPixel(std::move(new_pos.x), std::move(new_pos.y), c);
				}
			}
		}
	}

	void ChangePixel_Bloom(int x, int y, Color c)
	{
		size_t index = y * ScreenWidth + x;
		Bloom.PutPixel(index, c);
		PutPixel(index, c);
	}

	void DrawRectI_Bloom(RectI& rect, Color c)
	{
		for (int y = rect.top; y < rect.top + rect.height; y++)
		{
			for (int x = rect.left; x < rect.left + rect.width; x++)
				ChangePixel_Bloom(x, y, c);
		}
	}
	
	void ApplyBloom()
	{	
		blur_processor.Go();
		const Color* pColor = Bloom.Data();
		int r, g, b;
		for (size_t ind = 0; ind < ScreenHeight * ScreenWidth; ind++)
		{

			r = pSysBuffer[ind].GetR() + pColor[ind].GetR();
			if (r > 255)
			{
				r = 255;
			}
			g = pSysBuffer[ind].GetG() + pColor[ind].GetG();
			if (g > 255)
			{
				g = 255;
			}
			b = pSysBuffer[ind].GetB() + pColor[ind].GetB();
			if (r > 255)
			{
				b = 255;
			}

			pSysBuffer[ind] = Color(r, g, b);
		}
		
	}
	void ResetBloom()
	{
		Bloom.Fill(Colors::Black);
	}
	~Graphics();
private:
	BlurProcessor blur_processor = { Bloom };
	Sprite Bloom  = Sprite(ScreenWidth, ScreenHeight);
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;
	Color*                                              pSysBuffer = nullptr;
public:
	static constexpr int ScreenWidth = 800; 
	static constexpr int ScreenHeight = 600;
};