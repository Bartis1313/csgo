#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <cheats/classes/overrideView.hpp>
#include <cheats/classes/doPostScreen.hpp>

class IMaterial;

// or find it in post proccess for those params
class NoScope : protected RenderableSurfaceType
{
public:
	constexpr NoScope() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(NoScope);

class NoScopeBlur : protected DoPostScreenType
{
public:
	constexpr NoScopeBlur() :
		DoPostScreenType{}
	{}

protected:
	virtual void run() override;
	virtual void init() override;
private:
	IMaterial* m_blurScope;
	IMaterial* m_filterX;
	IMaterial* m_filterY;
	IMaterial* scope_lens_dirt;
};

GLOBAL_FEATURE(NoScopeBlur);

class CViewSetup;

class NoScopeBlurSnipers : protected OverrideViewType
{
public:
	constexpr NoScopeBlurSnipers() :
		OverrideViewType{}
	{}

protected:
	virtual void run(CViewSetup* v) override;
};

GLOBAL_FEATURE(NoScopeBlurSnipers);
