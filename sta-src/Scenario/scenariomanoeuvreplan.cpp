/*This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 details.
 
 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA 02111-1307, USA.
 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org. 
 */

/*
 ------ Copyright (C) 2008 European Space Agency (space.trajectory.analysis AT gmail.com) ----
 ------------------ Author: Chris Laurel  -------------------------------------------------
 ------------------ E-mail: (claurel@gmail.com) ----------------------------
 */

#include "Scenario/scenariomanoeuvreplan.h"
#include "Scenario/scenariomanoeuvre.h"
#include <QTreeWidgetItem>

ScenarioManoeuvrePlan::ScenarioManoeuvrePlan()
{		
}

ScenarioManoeuvrePlan::~ScenarioManoeuvrePlan(){
	
}

void ScenarioManoeuvrePlan::writeContents(QXmlStreamWriter* out){
	if(!m_manoeuvres.isEmpty()){
		foreach(ScenarioManoeuvre* manoeuvre, m_manoeuvres)
			manoeuvre->writeElement(out);
	}
}

void ScenarioManoeuvrePlan::createItemContents(QTreeWidgetItem* item){
	if(!m_manoeuvres.isEmpty()){
		foreach(ScenarioManoeuvre* manoeuvre, m_manoeuvres){
			if(manoeuvre->isActive())
				manoeuvre->createItem(item);
		}
	}
}

void ScenarioManoeuvrePlan::addManoeuvre(ScenarioManoeuvre* manoeuvre){
    Q_ASSERT(manoeuvre != NULL);
    Q_ASSERT(!manoeuvre->isAttached());
    attachChild(manoeuvre);
    m_manoeuvres.append(manoeuvre);
}

void ScenarioManoeuvrePlan::removeManoeuvre(ScenarioManoeuvre* manoeuvre){
    Q_ASSERT(manoeuvre != NULL);
    Q_ASSERT(manoeuvre->isAttached());
    detachChild(manoeuvre);
    m_manoeuvres.removeOne(manoeuvre);
}

