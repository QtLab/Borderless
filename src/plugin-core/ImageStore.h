/*
Copyright (c), Helios
All rights reserved.

Distributed under a permissive license. See COPYING.txt for details.
*/

#ifndef IMAGESTORE_H
#define IMAGESTORE_H

#include <unordered_map>
#include <memory>
#include <functional>
#include <cstdint>
#include <array>
#include <QImage>
#include "capi.h"

class Image;
class QString;

struct ImageOperationResult{
	bool success;
	int results[4];
	std::string message;
	ImageOperationResult(const char *msg = nullptr): success(!msg), message(msg ? msg : ""){}
};

enum class Trinary{
	Undefined,
	False,
	True,
};

struct SaveOptions{
	int compression;
	std::string format;
	SaveOptions(): compression(-1){}
};

typedef std::function<void(int, int, int, int, int, int)> traversal_callback;
typedef std::array<std::uint8_t, 4> pixel_t;

class ImageStore;

class Image{
	ImageStore *owner;
	int own_handle;
	QImage bitmap;
	bool alphaed;
	int w, h;
	static const unsigned stride = 4;
	unsigned pitch;
	std::uint8_t *current_pixel;

	void to_alpha();
public:
	Image(const QString &path, ImageStore &owner, int handle);
	Image(int w, int h, ImageStore &owner, int handle);
	Image(const QImage &, ImageStore &owner, int handle);
	void traverse(traversal_callback cb);
	void set_current_pixel(const pixel_t &rgba);
	ImageOperationResult save(const QString &path, SaveOptions opt);
	ImageOperationResult get_pixel(unsigned x, unsigned y);
	ImageOperationResult get_dimensions();
	void get_dimensions(int &w, int &h){
		w = this->w;
		h = this->h;
	}
	QImage get_bitmap() const{
		return this->bitmap;
	}
	ImageStore *get_owner() const{
		return this->owner;
	}
	int get_handle() const{
		return this->own_handle;
	}
	void *get_pixels_pointer(unsigned &stride, unsigned &pitch);
};

class ImageStore{
	std::unordered_map<int, std::shared_ptr<Image>> images;
	Image *current_traversal_image;
	int next_index;
public:
	ImageStore(): current_traversal_image(nullptr){}
	ImageOperationResult load(const char *path);
	ImageOperationResult load(const QString &path);
	Image *load_image(const char *path);
	int store(const QImage &);
	ImageOperationResult unload(int handle);
	void unload(Image *img){
		this->unload(img->get_handle());
	}
	ImageOperationResult save(int handle, const QString &path, SaveOptions opt);
	ImageOperationResult traverse(int handle, traversal_callback cb);
	ImageOperationResult allocate(int w, int h);
	Image *allocate_image(int w, int h);
	ImageOperationResult get_pixel(int handle, unsigned x, unsigned y);
	void set_current_pixel(const pixel_t &rgba);
	ImageOperationResult get_dimensions(int handle);

	Image *get_current_traversal_image(){
		return this->current_traversal_image;
	}
	void set_current_traversal_image(Image *image){
		this->current_traversal_image = image;
	}
	std::shared_ptr<Image> get_image(int img) const{
		auto i = this->images.find(img);
		if (i == this->images.end())
			return std::shared_ptr<Image>();
		return i->second;
	}
	void clear(){
		this->images.clear();
		this->next_index = 0;
	}
};

#define HANDLE_NOT_FOUND_MSG "Image handle doesn't exist."

#endif
