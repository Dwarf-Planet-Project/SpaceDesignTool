/*
 This program is free software; you can redistribute it and/or modify it under
 the terms of the European Union Public Licence - EUPL v.1.1 as published by
 the European Commission.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the European Union Public Licence - EUPL v.1.1
 for more details.

 You should have received a copy of the European Union Public Licence - EUPL v.1.1
 along with this program.

 Further information about the European Union Public Licence - EUPL v.1.1 can
 also be found on the world wide web at http://ec.europa.eu/idabc/eupl

*/

/*
 ------ Copyright (C) 2011 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/
/*
------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#ifndef _VISUALIZATION_FRAME_AXES_VISUALIZER_H_
#define _VISUALIZATION_FRAME_AXES_VISUALIZER_H_

#include <vesta/Visualizer.h>
#include <vesta/ArrowGeometry.h>
#include <vesta/Frame.h>

namespace vesta
{
class Entity;
}

class FrameAxesVisualizer : public vesta::Visualizer
{
public:
    FrameAxesVisualizer(vesta::Frame* frame, double size);
    virtual ~FrameAxesVisualizer();

    virtual Eigen::Quaterniond orientation(const vesta::Entity* parent, double t) const;

    vesta::ArrowGeometry* arrows();

    /** Enables/Disables the drawing of labels for an arrow specified by which
      */
    void setLabelEnabled(bool state, unsigned int which);

    /** Sets the text of the label for an arrow specified by which
      */
    void setLabelText(std::string text, unsigned int which);

    vesta::TextureFont* labelFont() const;
    void setLabelFont(vesta::TextureFont* font);

private:
    vesta::counted_ptr<vesta::Frame> m_frame;
};

#endif // _VISUALIZATION_FRAME_AXES_VISUALIZER_H_

