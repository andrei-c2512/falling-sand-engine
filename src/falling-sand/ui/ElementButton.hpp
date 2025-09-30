#include "engine2d/ui/Button.hpp"

#include "falling-sand/Weather.hpp"
//this file is so trash gang

namespace sand {
	class ElementButton : public e2d::Button
	{
	public:
		static constexpr int dim = 20;
		ElementButton(const e2d::Vec2I& pos, Type type0);
		Type GetType() const;
		void SelectSprite() ;
	private:
		Type type = Type::Empty;
	};
}
