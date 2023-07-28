#pragma once

namespace weather::helper
{
	enum class CustomPrecipitationType_t
	{
		PRECIPITATION_TYPE_NONE = -1,
		PRECIPITATION_TYPE_RAIN = 0,
		PRECIPITATION_TYPE_SNOW,
		PRECIPITATION_TYPE_PARTICLERAIN = 4,
		PRECIPITATION_TYPE_PARTICLERAINSTORM = 6,
		PRECIPITATION_TYPE_PARTICLESNOW,
		PRECIPITATION_TYPE_CUSTOM,
	};

	constexpr CustomPrecipitationType_t configToPrecip(int cfg)
	{
		switch (cfg)
		{
		case 1:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_RAIN;
		case 2:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_SNOW;
		case 3:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAIN;
		case 4:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLERAINSTORM;
		case 5:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_PARTICLESNOW;
		default:
			return CustomPrecipitationType_t::PRECIPITATION_TYPE_NONE;
		}
	}
}