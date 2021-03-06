/*
Copyright (c), Helios
All rights reserved.

Distributed under a permissive license. See COPYING.txt for details.
*/

#ifndef DIRECTORYLISTING_H
#define DIRECTORYLISTING_H

#include <QString>
#include <QStringList>
#include <QFuture>
#include <vector>

bool check_and_clean_path(QString &path);

class DirectoryIterator;

class DirectoryListing{
	bool ok;
	QString base_path;
	QFuture<QStringList> entries;
public:
	DirectoryListing(const QString &path);
	DirectoryIterator begin();
	size_t size();
	QString operator[](size_t) const;
	bool find(size_t &, const QString &) const;
	operator bool() const{
		return this->ok;
	}
	bool operator==(const QString &path) const;
};

class DirectoryIterator{
	DirectoryListing *dl;
	size_t position;
	bool in_position;
public:
	DirectoryIterator(DirectoryListing &dl): dl(&dl), position(0), in_position(false){}
	bool advance_to(const QString &name){
		if (this->in_position)
			return true;
		return this->in_position = this->dl->find(this->position, name);
	}
	QString operator*() const{
		return (*this->dl)[this->position];
	}
	void operator++(){
		this->position = (this->position + 1) % this->dl->size();
	}
	void operator--(){
		auto n = this->dl->size();
		this->position = (this->position + n - 1) % n;
	}
	DirectoryListing *get_listing() const{
		return this->dl;
	}
	size_t pos() const{
		return this->position;
	}
	void to_start(){
		this->position = 0;
	}
	void to_end(){
		this->to_start();
		--*this;
	}
};

#endif // DIRECTORYLISTING_H
