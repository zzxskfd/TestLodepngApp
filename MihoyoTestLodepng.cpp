// MihoyoTestLodePng.cpp : Contains the main functions that runs the application

#include "pch.h"
#include <iostream>
#include <cstdarg>
#include <thread>
#include <vector>
#include <filesystem>
#include <afxwin.h>
#include "lodepng.h"

clock_t start_time;

int thread_num;
std::vector<std::thread> threads;

std::vector<unsigned char> image; //the raw pixels
unsigned width, height;

CWnd* messageWindow;
CProgressCtrl* progressBar;
std::wstring wbuffer;

void warn_(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t len = std::vsnprintf(NULL, 0, format, args);
	va_end(args);
	std::vector<char> vec(len + 1);
	va_start(args, format);
	std::vsnprintf(&vec[0], len + 1, format, args);
	va_end(args);
	std::string str = &vec[0];
	std::wstring wstr(str.begin(), str.end());

	wbuffer += L"Warn: ";
	wbuffer += wstr;
	wbuffer += L"\r\n";
}


void info_(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t len = std::vsnprintf(NULL, 0, format, args);
	va_end(args);
	std::vector<char> vec(len + 1);
	va_start(args, format);
	std::vsnprintf(&vec[0], len + 1, format, args);
	va_end(args);
	std::string str = &vec[0];
	std::wstring wstr(str.begin(), str.end());

	wbuffer += L"Info: ";
	wbuffer += wstr;
	wbuffer += L"\r\n";
}

void flush() {
	messageWindow->SetWindowTextW(wbuffer.c_str());
	wbuffer.clear();
	messageWindow->UpdateWindow();
}

float get_time_elapsed() {
	return (float)(clock() - start_time) / CLOCKS_PER_SEC;
}

unsigned decodeOneStep(const char* filename) {
	info_("Start reading file %s", filename);
	image.clear();
	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	return error;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}

unsigned encodeOneStep(const char* filename) {
	info_("Start writing file %s", filename);
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	return error;
}

void process_file_subroutine(int height_start, int height_end) {
	int x_;
	for (unsigned y = height_start; y < height_end; y++) {
		for (unsigned x = 0; x < width / 2; x++) {
			x_ = width - 1 - x;
			std::swap(image[4 * width * y + 4 * x + 0], image[4 * width * y + 4 * x_ + 0]);
			std::swap(image[4 * width * y + 4 * x + 1], image[4 * width * y + 4 * x_ + 1]);
			std::swap(image[4 * width * y + 4 * x + 2], image[4 * width * y + 4 * x_ + 2]);
			std::swap(image[4 * width * y + 4 * x + 3], image[4 * width * y + 4 * x_ + 3]);
		}
	}
}

bool process_file()
{
	int min_per_thread = 25;
	int max_threads = (height + min_per_thread - 1) / min_per_thread;
	int hardware_threads = std::thread::hardware_concurrency();
	thread_num = min(hardware_threads ? hardware_threads : 2, max_threads);

	info_("Using %d threads", thread_num);

	int height_inteval = height / thread_num;
	threads = std::vector<std::thread>(thread_num);
	for (int i = 0; i < thread_num - 1; ++i) {
		threads[i] = std::thread(process_file_subroutine, height_inteval * i, height_inteval * (i + 1));
	}
	threads[thread_num - 1] = std::thread(process_file_subroutine, height_inteval * (thread_num - 1), height);

	for (int i = 0; i < thread_num; i++)
		threads[i].join();

	return true;
}

void init_progress_bar() {
	progressBar->SetRange(0, 1000);
	progressBar->SetPos(0);
}

float get_valid_file_size(std::filesystem::directory_entry file) {
	// Neglect files except those with extensions .png
	std::string ext = file.path().extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (ext.compare(".png"))
		return 0.;
	std::uintmax_t tmp = file.file_size();
	if (tmp != static_cast<std::uintmax_t>(-1))
		return tmp;
}

float get_total_file_size(std::filesystem::path folder) {
	float res = 0;
	for (const auto& file : std::filesystem::recursive_directory_iterator(folder)) {
		float tmp = get_valid_file_size(file);
		res += get_valid_file_size(file);
	}
	return res;
}

void set_progress_bar(int nPos) {
	progressBar->SetPos(nPos);
	progressBar->UpdateWindow();
}

void run_flip_png(
	std::filesystem::path input_folder_,
	std::filesystem::path output_folder_,
	CWnd* messageWindow_,
	CProgressCtrl* progressBar_)
{
	std::filesystem::path input_folder;
	std::filesystem::path output_folder;
	input_folder = input_folder_;
	output_folder = output_folder_;
	messageWindow = messageWindow_;
	progressBar = progressBar_;
	// Set range of the progress bar to [0, 1000].
	float total_file_size = get_total_file_size(input_folder);
	float done_file_size = 0;
	init_progress_bar();
	// Start
	start_time = clock();
	// Create output folder.
	std::filesystem::create_directories(output_folder);
	for (const auto& file : std::filesystem::recursive_directory_iterator(input_folder)) {
		flush();
		std::filesystem::path input_path = file.path();
		// If have subfolder, create corresponding subfolder.
		if (std::filesystem::is_directory(input_path)) {
			std::filesystem::create_directories(output_folder / relative(input_path, input_folder));
			continue;
		}
		// If not extended with ".png", file size will be 0
		float cur_file_size = get_valid_file_size(file);
		if (cur_file_size < 1)
			continue;
		// Generate output filename.
		std::filesystem::path output_path = output_folder / relative(input_path, input_folder);
		output_path.replace_filename(output_path.filename().stem().string() + std::string("_flipped") + output_path.extension().string());
		// Read, process and write.
		bool succeeded = false;
		if (!decodeOneStep(input_path.string().c_str()))
			if (process_file())
				succeeded = !encodeOneStep(output_path.string().c_str());
		// Calculate progress
		if (succeeded) {
			done_file_size += cur_file_size;
		}
		else {
			total_file_size -= cur_file_size;
			total_file_size = max(total_file_size, 1e-8);		// Avoid dividing by 0
		}
		set_progress_bar(min(done_file_size / total_file_size * 1000.0, 1000.0));
		info_("Time elapsed: %.3fs\n", get_time_elapsed());
	}
	set_progress_bar(1000);
	info_("...done!");
	flush();
}
