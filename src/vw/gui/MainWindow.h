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


/// \file vwv_MainWindow.h
///
/// The Vision Workbench image viewer main window class.
///
#ifndef __VWV_MAINWINDOW_H__
#define __VWV_MAINWINDOW_H__

#include <QMainWindow>
#include <string>

// Boost
#include <boost/program_options.hpp>

// Forward declarations
class QAction;
class QLabel;
class QTabWidget;

namespace vw {
namespace gui {

  class GlPreviewWidget;

  class MainWindow : public QMainWindow {
    Q_OBJECT

    std::string m_filename;
    float m_nodata_value;
    boost::program_options::variables_map const& m_vm;

  public:
    MainWindow(std::string filename, float nodata_value, int transaction_id, bool do_normalize, boost::program_options::variables_map const& vm);
    virtual ~MainWindow() {}

  private slots:
    void about();
    void update_status_bar(std::string const& s);

  protected:
    void keyPressEvent(QKeyEvent *event);

  private:
    void create_actions();
    void create_menus();
    void create_status_bar();

    GlPreviewWidget *m_preview_widget;

    QMenu *file_menu;
    QMenu *edit_menu;
    QMenu *help_menu;

    QLabel *status_label;
    QAction *about_action;
    QAction *exit_action;
  };

}} // namespace vw::gui

#endif // __VWV_MAINWINDOW_H__
