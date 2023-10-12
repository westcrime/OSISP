#pragma once

class SystemInfo
{
public:
	SystemInfo();
	void static GetWorkloadInfo(double& value);
	void static GetMemoryInfo(double& value);
};