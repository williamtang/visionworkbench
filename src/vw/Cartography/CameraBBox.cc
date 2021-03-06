// __BEGIN_LICENSE__
//  Copyright (c) 2006-2013, United States Government as represented by the
//  Administrator of the National Aeronautics and Space Administration. All
//  rights reserved.
//
//  The NASA Vision Workbench is licensed under the Apache License,
//  Version 2.0 (the "License"); you may not use this file except in
//  compliance with the License. You may obtain a copy of the License at
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// __END_LICENSE__


#include <vw/Cartography/CameraBBox.h>

using namespace vw;


// Intersect the ray back-projected from the camera with the datum.
Vector3
cartography::datum_intersection( Datum const& datum,
                                 Vector3 camera_ctr, Vector3 camera_vec) {

  // The datum is a spheroid. To simplify the calculations, scale
  // everything in such a way that the spheroid becomes a
  // sphere. Scale back at the end of computation.

  double z_scale = datum.semi_major_axis() / datum.semi_minor_axis();
  camera_ctr.z() *= z_scale;
  camera_vec.z() *= z_scale;
  camera_vec = normalize(camera_vec);
  double radius_2 = datum.semi_major_axis() *
    datum.semi_major_axis();
  double alpha = -dot_prod(camera_ctr, camera_vec );
  Vector3 projection = camera_ctr + alpha*camera_vec;
  if ( norm_2_sqr(projection) > radius_2 ) {
    // did not intersect
    return Vector3();
  }

  alpha -= sqrt( radius_2 -
                 norm_2_sqr(projection) );
  Vector3 intersection = camera_ctr + alpha * camera_vec;
  intersection.z() /= z_scale;
  return intersection;
}

Vector3
cartography::datum_intersection( Datum const& datum,
                                 camera::CameraModel const* model,
                                 Vector2 const& pix ) {
  return datum_intersection(datum, model->camera_center(pix), model->pixel_to_vector(pix));
}

// Return the intersection between the ray emanating from the
// current camera pixel with the datum ellipsoid. The return value
// is a map projected point location (the intermediate between
// lon-lat-altitude and pixel).
Vector2
cartography::geospatial_intersect(cartography::GeoReference const& georef,
                                  Vector3 const& camera_ctr, Vector3 const& camera_vec,
                                  bool& has_intersection) {

  Vector3 intersection = datum_intersection(georef.datum(), camera_ctr, camera_vec);
  if (intersection == Vector3()){
    has_intersection = false;
    return Vector2();
  } else {
    has_intersection = true;
  }

  Vector3 llh = georef.datum().cartesian_to_geodetic( intersection );
  Vector2 projected_point = georef.lonlat_to_point( Vector2( llh.x(),
                                                             llh.y() ) );

  return projected_point;
}

// Compute the bounding box in points (georeference space) that is
// defined by georef. Scale is MPP as georeference space is in meters.
BBox2 cartography::camera_bbox( cartography::GeoReference const& georef,
                                boost::shared_ptr<camera::CameraModel> camera_model,
                                int32 cols, int32 rows, float &scale ) {

  // Testing to see if we should be centering on zero
  bool center_on_zero = true;
  Vector3 camera_llr =
    XYZtoLonLatRadFunctor::apply(camera_model->camera_center(Vector2()));
  if ( camera_llr[0] < -90 ||
       camera_llr[0] > 90 )
    center_on_zero = false;

  int32 step_amount = (2*cols+2*rows)/100;
  detail::CameraDatumBBoxHelper functor( georef, camera_model,
                                         center_on_zero );

  // Running the edges
  bresenham_apply( BresenhamLine(0,0,cols,0),
                   step_amount, functor );
  functor.last_valid = false;
  bresenham_apply( BresenhamLine(cols-1,0,cols-1,rows),
                   step_amount, functor );
  functor.last_valid = false;
  bresenham_apply( BresenhamLine(cols-1,rows-1,0,rows-1),
                   step_amount, functor );
  functor.last_valid = false;
  bresenham_apply( BresenhamLine(0,rows-1,0,0),
                   step_amount, functor );
  functor.last_valid = false;

  // Running once through the center
  bresenham_apply( BresenhamLine(0,0,cols,rows),
                   step_amount, functor );

  scale = functor.scale/double(step_amount);
  return functor.box;
}

void cartography::detail::CameraDatumBBoxHelper::operator()( Vector2 const& pixel ) {
  bool has_intersection;
  Vector2 point =
    geospatial_intersect(m_georef,
                         m_camera->camera_center(pixel),
                         m_camera->pixel_to_vector(pixel),
                         has_intersection);
  if ( !has_intersection ) {
    last_valid = false;
    return;
  }

  if (!m_georef.is_projected()){
    // If we don't use a projected coordinate system, then the
    // coordinates of this point are simply lon and lat.
    if ( center_on_zero && point[0] > 180 )
      point[0] -= 360.0;
  }

  if ( last_valid ) {
    double current_scale =
      norm_2( point - m_last_intersect );
    if ( current_scale < scale )
      scale = current_scale;
  }
  m_last_intersect = point;
  box.grow( point );
  last_valid = true;
}
