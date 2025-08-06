#ifndef APP_STDAFX_H
#define APP_STDAFX_H

#include <QtGui/QtGui>
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

#include <memory>
#include <functional>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <type_traits>
#include <iostream>

#include "lite/factory.h"
#include "lite/pridata.h"
#include "lite/qtlog.h"
#include "lite/tools.h"

#include "xutility/utility.h"

using ItemRoleMap = QMap<int, QVariant>;

#endif //APP_STDAFX_H