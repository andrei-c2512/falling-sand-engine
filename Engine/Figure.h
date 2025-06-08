#pragma once
#include "Graphics.h"
#include "RNG.h"
#include <map>
#include "Camera.h"
#include "Drawable.h"
#include "Rect.h"
#include "Effects.h"
class Romb {
public:
	static std::vector<Vec2I> GetRomb(int length = 5)
	{
		std::vector<Vec2I> poli = { Vec2I(length ,  0) ,
									Vec2I(0 , -length) ,
									Vec2I(-length , 0),
									Vec2I(0 , length) };
	    
		
		return poli;
	}
};

class Square {
public:
	static std::vector<Vec2I> GetSquare(int length = 5)
	{
		std::vector<Vec2I> poli = { Vec2I(length ,  length) ,
									Vec2I(length , -length) ,
									Vec2I(-length , -length),
									Vec2I(-length , length) };


		return poli;
	}
};
class XFigure {
public:
	static std::vector<Vec2I> GetPoly(int length = 10)
	{
		std::vector<Vec2I> poli = { Vec2I(length , 0) ,
									Vec2I(-length , 0) ,
									Vec2I(0 , length),
									Vec2I(0 , -length) };


	}
};


class Star {
public:
	static std::vector<Vec2I> GetStar(int flares , int flare_lenght = 5 , int flare_deepness = 4)
	{
		std::vector<Vec2I> poli;

		assert(flare_lenght > flare_deepness);

		float flare_angle = 360.0f / float(flares);
		float portrusion_angle = 360.0f / (float(flares) * 2.0f);

		float flare_dist = flare_lenght - flare_deepness;
		for (float angle = 0; angle < 360; angle += flare_angle)
		{
			float cos = std::cos(angle * PI / 180.0f);
			float sin = std::sin(angle * PI / 180.0f);

			Vec2I pos(cos * flare_lenght, sin * flare_lenght);
			poli.emplace_back(pos);

			cos = std::cos((angle + portrusion_angle) * PI / 180.0f);
			sin = std::sin((angle + portrusion_angle) * PI / 180.0f);

			pos = Vec2I(cos * flare_dist, sin * flare_dist);

			poli.emplace_back(pos);
		}

		return poli;
	}
};




class Figure {
public:
	enum class ScaleState {
		Growing,
		Shrinking
	};
public:
	enum class ColorState {
		Darkening,
		Blooming
	};
public:
	enum class ColorShift {
		Red, 
		Green, 
		Blue
	};
public:
	Figure(Vec2I pos0)
		:pos(std::move(pos0))
	{
		//getting the poligon
		RNG flare_rng = { 5 , 15 };

		poli = Star::GetStar(flare_rng.GetVal(), 100, 50);

	    // poli = Square::GetSquare(100);
		// arranging the scaling , its shrinking and growing speed and limt
		RNG Max_rng = { 7 , 13 };
		RNG Min_rng = { 2 , 3 };
		RNG ScaleFactor_rng = { 50 , 250 };
		RNG Rotation_rng = { 1 , 12 };

		AnglePerSecond = PI / Rotation_rng.GetVal();
		MinScale = Min_rng.GetVal();
		MaxScale = Max_rng.GetVal();

		Scale = 10.0f;
		ScaleIncreaseFactor = ScaleFactor_rng.GetVal() / 50.0f;

		scale_state = ScaleState::Growing;

		//setting its color change pattern
		RNG ColorFactor_rng = { 5 , 50 };
		RNG Color_rng = { 0 , 2 };
		RNG RGB_rng = { 0 , 255 };

		shifting_color = ColorShift(Color_rng.GetVal());

		R = RGB_rng.GetVal();
		G = RGB_rng.GetVal();
		B = RGB_rng.GetVal();

		RGB_map.emplace(ColorShift::Red  , R);
		RGB_map.emplace(ColorShift::Green, G);
		RGB_map.emplace(ColorShift::Blue , B);

	}
	void Draw(Camera& cam) 
	{
		Drawable dr(GetRotatedModel());
		dr.ApplyScale(Scale);
		dr.Translate(pos);

		cam.Draw(dr, Color(int(R) ,int(G) ,int(B)));
	}
	void DrawHitBox(Graphics& gfx , Camera& cam)
	{
		auto CurrentPoli = GetCurrentModelSize();

		RectI HitBox = RectI::GetRect(CurrentPoli);
		HitBox.left += pos.x;
		HitBox.bottom += pos.y;
		gfx.DrawRect_Border(cam.Transform(std::move(HitBox)), Colors::Cyan, Effects::Copy{});
	}
	std::vector<Vec2I> GetRotatedModel()
	{
		auto rotated_poli = poli;
		for (Vec2I& point : rotated_poli)
		{
			point.Rotate(Theta);
		}
		return rotated_poli;
	}
	std::vector<Vec2I>& GetModel()
	{
		return poli;
	}
	std::vector<Vec2I> GetCurrentModelSize() const {
		std::vector<Vec2I> new_model = poli;

		for (Vec2I& vertices : new_model)
		{
			vertices *= Scale;
		}

		return new_model;
	}
	std::vector<Vec2I> GetMaxModelSize() const 
	{
		std::vector<Vec2I> new_model = poli;

		for (Vec2I& vertices : new_model)
		{
			vertices *= MaxScale;
		}

		return new_model;
	}
	void SetPos(Vec2I pos0)
	{
		pos = std::move(pos0);
	}
	void Update(float time)
	{
		if (scale_state == ScaleState::Growing)
		{
			Scale += ScaleIncreaseFactor * time;
			if (int(Scale) >= MaxScale)
			{
				Scale = MaxScale;
				scale_state = ScaleState::Shrinking;
			}
		}
		else if(scale_state == ScaleState::Shrinking)
		{
			Scale -= ScaleIncreaseFactor * time;
			if (float(Scale) < MinScale)
			{
				Scale = MinScale;
				scale_state = ScaleState::Growing;
			}
		}
		UpdateColor(time);

		Theta += time * AnglePerSecond;
	}
	void Rotate(float time)
	{
		//rotation factor
		float Theta = (PI / 6.0f) * time;
		for (Vec2I& point : poli)
		{
			//point.Rotate(Theta);
		}
	}
	Vec2I GetPos() const {
		return pos;
	}
	int GetX() const {
		return pos.x;
	}
	int GetY() const {
		return pos.y;
	}
private:
	void UpdateColor(float time )
	{
		ChangeColor(RGB_map[shifting_color], time);
	}
	void ChangeColor(float value , float time)
	{
		assert(value >= 0 && value < 256);
		if (color_state == Figure::ColorState::Blooming)
		{
			value += ColorIncreaseFactor * time;
			if (value >= 255.0f)
			{
				value = 255.0f;
				color_state = Figure::ColorState::Darkening;
			}
		}
		else
		{
			value -= ColorIncreaseFactor * time;
			if (value <= 0.0f)
			{
				value = 0.0f;
				color_state = Figure::ColorState::Blooming;
			}
		}
	}
private:
	std::map<ColorShift, float> RGB_map;
	std::vector<Vec2I> poli;
	Vec2I pos;
	//scaling

	float Scale = 0;
	int MinScale = 0;
	int MaxScale = 0;
	float ScaleIncreaseFactor = 0.0f;
	ScaleState scale_state;

	//color changing varaibles
	float ColorIncreaseFactor = 0.0f;
	float R;
	float G;
	float B;

	ColorState color_state;
	ColorShift shifting_color;

	float Theta = 0.0f;
	float AnglePerSecond = 0.0f;
};


class SpaceChunk {
public:
	SpaceChunk(const RectI& area)
		:surface(area)
	{
	}
	void AddToList(Figure& figure)
	{
		figure_list.emplace_back(figure);
	}
	bool CheckCollisionForSpawning(RectI& rect) const
	{
		for (const Figure& fig : figure_list)
		{
			RectI HitBox = RectI::GetRect(fig.GetMaxModelSize());
			HitBox.left += fig.GetX();
			HitBox.bottom += fig.GetY();

			if (rect.Collision(std::move(HitBox)))
			{
				return true;
			}
		}
		return false;
	}
	const Figure* CheckBoxCollision(RectI& rect) const
	{
		for (const Figure& fig : figure_list)
		{
			RectI HitBox = RectI::GetRect(fig.GetCurrentModelSize());
			HitBox.left += fig.GetX();
			HitBox.bottom += fig.GetY();

			if (rect.Collision(HitBox))
			{
				return const_cast<const Figure*> (&fig);
			}
		}

		return nullptr;
	}
	void DrawFigures(Graphics& gfx ,Camera& camera)
	{
		//if(surface.Collision(camera.GetRect()))
		for (Figure& fig : figure_list)
		{
			fig.Draw(camera);
			fig.DrawHitBox(gfx, camera);
		}
	}
	void Update(float time)
	{
		for (Figure& fig : figure_list)
		{
			fig.Update(time);
		}
	}
	void DrawBorder(Camera& camera, Graphics& gfx)
	{
		gfx.DrawRect_Border(camera.Transform(surface), Colors::Red, Effects::Copy{});
	}
	const std::vector<Figure>& GetFigure_list() const {
		return figure_list;
	}
private:
	RectI surface;
	std::vector<Figure> figure_list;
};

class FigureField {
public:
	FigureField()
	{
		rows    = Graphics::WorldHeight / chunk_dim.height;
		columns = Graphics::WorldWidth  / chunk_dim.width ;

		for (int y = 0; y < rows; y++)
		{
			for (int x = 0; x < columns; x++)
			{
				RectI rect = RectI(chunk_dim, Vec2I(x * chunk_dim.width - Graphics::WorldWidth / 2 ,
								                    y * chunk_dim.height - Graphics::WorldHeight / 2));
				chunk_list.emplace_back(std::move(rect));
			}
		}
			
		for (unsigned int i = 0; i < limit ; i++)
		{
			SpawnFigure();
		}
	}
	void DrawFigureField(Camera& cam , Graphics& gfx)
	{
		for (SpaceChunk& chunk : chunk_list)
		{
			chunk.DrawFigures(gfx ,cam);

			//chunk.DrawBorder(cam, gfx);
		}

	}
	void Update(float time)
	{
		for (SpaceChunk& chunk : chunk_list)
		{
			chunk.Update(time);
		}
	}
private:
	void SpawnFigure()
	{
		bool FoundPosition = false;

		Figure fig = Figure(Vec2I(0, 0));

		std::vector<Vec2I> model = fig.GetMaxModelSize();
		RectI NewFigHitBox = RectI::GetRect(model);

		RNG PosX = { Graphics::WorldArea.left   + NewFigHitBox.width , Graphics::WorldArea.right() - NewFigHitBox.width };
		RNG PosY = { Graphics::WorldArea.bottom + NewFigHitBox.height, Graphics::WorldArea.top()   - NewFigHitBox.height };

		do {
			FoundPosition = true;
			
			RectI HitBox = NewFigHitBox;
			HitBox.left   += PosX.GetVal();
			HitBox.bottom += PosY.GetVal();

			for (const SpaceChunk& chunk : chunk_list)
			{
				FoundPosition = !(chunk.CheckCollisionForSpawning(HitBox));
				if (FoundPosition == false)
				{
					break;
				}
			}

			if (FoundPosition)
			{
				fig.SetPos(HitBox.GetPos());
			}

		} while (FoundPosition == false);

		Vec2I fig_pos = fig.GetPos() + Vec2I(columns / 2 * chunk_dim.width , rows / 2 * chunk_dim.height);
		Vec2I chunk_pos = Vec2I(fig_pos.x / chunk_dim.width , fig_pos.y / chunk_dim.height );


		unsigned int index = chunk_pos.y * columns + chunk_pos.x;
		chunk_list[index].AddToList(std::move(fig));
	}
public:
	Dimensions<short> GetChunkDim() const {
		return chunk_dim;
	}
	short GetChunkWidth() const {
		return chunk_dim.width;
	}
	short GetChunkHeight() const {
		return chunk_dim.height;
	}
	SpaceChunk* ChunkListData()
	{
		return chunk_list.data();
	}
	int GetRows() const {
		return rows;
	}
	int GetColumns() const {
		return columns;
	}
private:
	Dimensions<short> chunk_dim = { Graphics::ScreenWidth , Graphics::ScreenHeight };
	int rows;
	int columns;
	std::vector<SpaceChunk> chunk_list;
	unsigned int limit = 20;

};