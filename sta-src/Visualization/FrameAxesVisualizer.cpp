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

/*
 * $Revision: 575 $ $Date: 2011-03-16 16:39:49 -0700 (Wed, 16 Mar 2011) $
 *
 * Copyright by Astos Solutions GmbH, Germany
 *
 * this file is published under the Astos Solutions Free Public License
 * For details on copyright and terms of use see
 * http://www.astos.de/Astos_Solutions_Free_Public_License.html
 */

#include "FrameAxesVisualizer.h"

using namespace vesta;
using namespace Eigen;


FrameAxesVisualizer::FrameAxesVisualizer(Frame* frame, double size) :
    Visualizer(NULL),
    m_frame(frame)
{
    ArrowGeometry* geometry = new ArrowGeometry(0.9f, 0.01f, 0.1f, 0.02f);
    geometry->setScale(size);
    geometry->setVisibleArrows(ArrowGeometry::AllAxes);
    setGeometry(geometry);
}


FrameAxesVisualizer::~FrameAxesVisualizer()
{
}


Eigen::Quaterniond
FrameAxesVisualizer::orientation(const Entity* parent, double t) const
{
    return m_frame->orientation(t);
}


ArrowGeometry*
FrameAxesVisualizer::arrows()
{
    return dynamic_cast<ArrowGeometry*>(geometry());
}


/** Set the visibility of and axis specified by which
  */
void FrameAxesVisualizer::setLabelEnabled(bool state, unsigned int which)
{
    ArrowGeometry* arrow = dynamic_cast<ArrowGeometry*>( geometry() );
    arrow->setLabelEnabled(state, which);
}

/** Sets the label text for an arrow specified by which
  */
void FrameAxesVisualizer::setLabelText(std::string text, unsigned int which)
{
    ArrowGeometry* arrow = dynamic_cast<ArrowGeometry*>( geometry() );
    arrow->setLabelText(text, which);
}


/** Get the font used for the label text.
  */
TextureFont*
FrameAxesVisualizer::labelFont() const
{
    ArrowGeometry* arrow = dynamic_cast<ArrowGeometry*>( geometry() );
    if (arrow)
    {
        return arrow->labelFont();
    }
    else
    {
        return NULL;
    }
}


/** Sets a font for the label text.
  */
void
FrameAxesVisualizer::setLabelFont(TextureFont* font)
{
    ArrowGeometry* arrow = dynamic_cast<ArrowGeometry*>( geometry() );
    if (arrow)
    {
        arrow->setLabelFont(font);
    }
}

