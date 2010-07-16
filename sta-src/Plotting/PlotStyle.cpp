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
 ------ Copyright (C) 2010 STA Steering Board (space.trajectory.analysis AT gmail.com) ----
*/

//------------------ Author:       Chris Laurel                   -------------------
//-----------------------------------------------------------------------------------

#include "PlotStyle.h"


PlotStyle::PlotStyle(PlotType type) :
    m_type(type),
    m_strokeStyle(QPen(Qt::red))
{
}


PlotStyle::PlotStyle(PlotType type, const QPen strokeStyle) :
    m_type(type),
    m_strokeStyle(strokeStyle)
{
}


PlotStyle::PlotStyle(const PlotStyle& other) :
    m_type(other.m_type),
    m_strokeStyle(other.m_strokeStyle)
{
}


PlotStyle& PlotStyle::operator=(const PlotStyle& other)
{
    m_type = other.m_type;
    m_strokeStyle = other.m_strokeStyle;
    return *this;
}
