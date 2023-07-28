#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>

class IMaterial;
enum FrameStage;

// DO NOT USE LOCKS HERE
namespace weather::ground
{
	using groundInfo = std::unordered_map<std::string_view, std::vector<std::string_view>>;

	groundInfo getGroundInfo();

	void reset();
	// CRITICAL SECTION
	void shutdown();
	void resetFindMat([[maybe_unused]] FrameStage stage);
	void init();
	void create();
	void findIndexes();
	// CRITICAL SECTION
	void applyManually();
	IMaterial* applyFindMat(IMaterial* mat);

	inline bool calledShutdown = false;
	inline bool calledCreate = false;
}