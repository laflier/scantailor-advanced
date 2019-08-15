/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILTERDATA_H_
#define FILTERDATA_H_

#include <QImage>
#include "ImageSettings.h"
#include "ImageTransformation.h"
#include <BinaryThreshold.h>
#include <GrayImage.h>

class FilterData {
  // Member-wise copying is OK.
 public:
  explicit FilterData(const QImage& image);

  FilterData(const FilterData& other, const ImageTransformation& xform);

  FilterData(const FilterData& other);

  imageproc::BinaryThreshold bwThreshold() const;

  const ImageTransformation& xform() const;

  const QImage& origImage() const;

  const imageproc::GrayImage& grayImage() const;

  bool isBlackOnWhite() const;

  void updateImageParams(const ImageSettings::PageParams& imageParams);

 private:
  QImage m_origImage;
  imageproc::GrayImage m_grayImage;
  ImageTransformation m_xform;
  ImageSettings::PageParams m_imageParams;
};


#endif  // ifndef FILTERDATA_H_