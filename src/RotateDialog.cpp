/*
Copyright (c), Helios
All rights reserved.

Distributed under a permissive license. See COPYING.txt for details.
*/

#include "RotateDialog.h"
#include "Misc.h"
#include <cmath>

const double log_125 = log(1.25);

RotateDialog::RotateDialog(MainWindow &parent) :
		QDialog(parent.centralWidget()),
		main_window(parent),
		ui(new Ui_RotateDialog),
		result(false),
		in_do_transform(false){
	this->setModal(true);
	this->ui->setupUi(this);
	this->transform = parent.get_image_transform();
	connect(this->ui->rotation_slider, SIGNAL(valueChanged(int)), this, SLOT(rotation_slider_changed(int)));
	connect(this->ui->scale_slider, SIGNAL(valueChanged(int)), this, SLOT(scale_slider_changed(int)));
	connect(this->ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejected_slot()));
	this->last_scale = this->original_scale = this->scale = this->main_window.get_image_zoom();
	this->rotation_slider_changed(0);
	this->set_scale();
}

void RotateDialog::do_transform(bool set_zoom){
	auto scale = this->main_window.set_image_transform(this->transform * QMatrix().rotate(this->rotation));
	if (!this->main_window.current_zoom_mode_is_auto() || set_zoom && !this->in_do_transform)
		this->main_window.set_image_zoom(this->scale);
	else{
		this->in_do_transform = true;
		this->scale = scale;
		this->set_scale();
		this->in_do_transform = false;
	}
}

void RotateDialog::set_scale(){
	this->ui->scale_slider->setValue((int)(log(this->scale) / log_125 * 1000.0));
	this->set_scale_label();
}

void RotateDialog::rotation_slider_changed(int value){
	double theta = value / 100.0;
	QString s = QString::fromStdString(itoac(theta));
	s += " deg";
	this->ui->rotation_label->setText(s);
	this->rotation = theta;
	this->do_transform();
}

void RotateDialog::set_scale_label(){
	QString s = QString::fromStdString(itoac(this->scale));
	s += "x";
	this->ui->scale_label->setText(s);
}

void RotateDialog::scale_slider_changed(int value){
	double x = value / 1000.0;
	this->scale = pow(1.25, x);
	this->set_scale_label();
	this->do_transform(true);
}

void RotateDialog::rejected_slot(){
	this->rotation = 0;
	this->scale = this->original_scale;
	this->do_transform();
}
