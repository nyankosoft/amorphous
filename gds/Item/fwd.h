#ifndef  __GAMEITEM_FWD_H__
#define  __GAMEITEM_FWD_H__


class CGameItem;


#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<CGameItem> CGameItemSharedPtr;
typedef boost::weak_ptr<CGameItem> CGameItemWeakPtr;


#endif /* __GAMEITEM_FWD_H__ */
