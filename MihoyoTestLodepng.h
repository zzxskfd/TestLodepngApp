#pragma once
#ifndef MIHOYOTESTLODEPNG_H
#define MIHOYOTESTLODEPNG_H

#include <filesystem>
#include <iostream>
#include <afxwin.h>

extern void run_flip_png(
	std::filesystem::path input_folder_,
	std::filesystem::path output_folder_,
	CWnd* messageWindow,
	CProgressCtrl* progressBar);

#endif