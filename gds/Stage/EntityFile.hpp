#ifndef  __ENTITYFILE_H__
#define  __ENTITYFILE_H__


namespace amorphous
{

struct SEntityFileHeader
{
	int iCopyEntityDescSize;
	int iCopyEntitySize;
	int iNumCopyEntities;
};

} // namespace amorphous


#endif  /*  __ENTITYFILE_H__  */