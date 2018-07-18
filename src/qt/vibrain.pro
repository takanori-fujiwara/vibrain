#-------------------------------------------------
#
# Project created by QtCreator 2016-11-02T17:46:23
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vibrain
TEMPLATE = app


SOURCES += main.cpp\
        main_window.cpp \
    brain_mesh.cpp \
    roi.cpp \
    rois.cpp \
    scanned_data.cpp \
    scanned_dataset.cpp \
    dim_reduction.cpp \
    comparison_view.cpp \
    style.cpp \
    matrix_view.cpp \
    brain_graph_view.cpp \
    detailed_matrix.cpp \
    setting_panel.cpp \
    information_view.cpp \
    threshold_control_view.cpp \
    edge_bundle_connections.cpp \
    edge_bundle_edge.cpp \
    community_detection.cpp \
    hierarchical_clustering.cpp \
    data_directory_selection_dialog.cpp \
    subset_rois_setting_dialog.cpp \
    threshold_setting_dialog.cpp \
    value_interpolation.cpp

HEADERS  += main_window.hpp \
    brain_mesh.hpp \
    roi.hpp \
    rois.hpp \
    scanned_data.hpp \
    scanned_dataset.hpp \
    dim_reduction.hpp \
    comparison_view.hpp \
    style.hpp \
    matrix_view.hpp \
    brain_graph_view.hpp \
    detailed_matrix.hpp \
    setting_panel.hpp \
    information_view.hpp \
    threshold_control_view.hpp \
    edge_bundle_connections.hpp \
    edge_bundle_edge.hpp \
    community_detection.hpp \
    hierarchical_clustering.hpp \
    data_directory_selection_dialog.hpp \
    subset_rois_setting_dialog.hpp \
    threshold_setting_dialog.hpp \
    value_interpolation.hpp

FORMS    += main_window.ui

CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE += -O3 # to optimize the compiled result
QMAKE_CLEAN +=		qtdensity Makefile
ICON = vibrain.icns

################
#R_HOME = $$system("R R_HOME")
#R_HOME = $$PWD/R.framework/Versions/Current/Resources
#message("R_HOME is" $$R_HOME)

#QMAKE_LFLAGS += -F $$PWD
#LIBS += -framework R

#INCLUDEPATH += $$PWD/R.framework/Versions/Current/Resources/include
#DEPENDPATH += $$PWD/R.framework/Versions/Current/Resources/include
#LIBS += -L$$PWD/R.framework/Versions/Current/Resources/lib -lR
#LIBS += -L$$PWD/R.framework/Versions/Current/Resources/lib -lRblas
#LIBS += -L$$PWD/R.framework/Versions/Current/Resources/lib -lRlapack

#INCLUDEPATH += $$PWD/RInside/include
#DEPENDPATH += $$PWD/RInside/include
#LIBS += -L$$PWD/RInside/lib -lRInside

#INCLUDEPATH += $$PWD/Rcpp/include
#DEPENDPATH += $$PWD/Rcpp/include
#LIBS += -L$$PWD/Rcpp/libs/Rcpp.so

###########
## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
RHOME = 		$$system(/usr/local/bin/R RHOME)
unix:!macx {
    RHOME = $$system(/usr/lib/R RHOME)
    LIBS += -lglut -lGLU
}

#R_HOME = $$PWD/R.framework/Versions/Current/Resources

# include headers and libraries for R
RCPPFLAGS = 		$$system($$RHOME/bin/R CMD config --cppflags)
RLDFLAGS = 		$$system($$RHOME/bin/R CMD config --ldflags)
RBLAS = 		$$system($$RHOME/bin/R CMD config BLAS_LIBS)
RLAPACK = 		$$system($$RHOME/bin/R CMD config LAPACK_LIBS)

# if you need to set an rpath to R itself, also uncomment
RRPATH =		-Wl,-rpath,$$RHOME/lib

# include headers and libraries for Rcpp interface classes
# note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL = 		$$system($$RHOME/bin/Rscript -e \"Rcpp:::CxxFlags\(\)\")
RCPPLIBS = 		$$system($$RHOME/bin/Rscript -e \"Rcpp:::LdFlags\(\)\")

# for some reason when building with Qt we get this each time
#   /usr/local/lib/R/site-library/Rcpp/include/Rcpp/module/Module_generated_ctor_signature.h:25: warning: unused parameter ‘classname
# so we turn unused parameter warnings off
# no longer needed with Rcpp 0.9.3 or later
RCPPWARNING =		-Wno-unused-parameter

# include headers and libraries for RInside embedding classes
RINSIDEINCL = 		$$system($$RHOME/bin/Rscript -e \"RInside:::CxxFlags\(\)\")
RINSIDELIBS = 		$$system($$RHOME/bin/Rscript -e \"RInside:::LdFlags\(\)\")

# compiler etc settings used in default make rules
QMAKE_CXXFLAGS +=	$$RCPPWARNING $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL
QMAKE_LIBS +=           $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS

# to use RHOME in cpp file.
DEFINES += RHOME=\\\"$$RHOME\\\"
