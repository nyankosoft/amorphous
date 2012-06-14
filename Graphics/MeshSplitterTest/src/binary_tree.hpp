#ifndef __binary_tree_HPP__
#define __binary_tree_HPP__


#include <boost/shared_ptr.hpp>


template<typename T>
class binary_node
{
public:

	binary_node(){}

	binary_node( boost::shared_ptr<T>& obj ) : object(obj) {}

	~binary_node(){}

	T object;

	boost::shared_ptr< binary_node<T> > child0;

	boost::shared_ptr< binary_node<T> > child1;

public:

	unsigned int get_num_nodes() const
	{
		return 1
			+ (unsigned int)(child0 ? child0->get_num_nodes() : 0)
			+ (unsigned int)(child1 ? child1->get_num_nodes() : 0);
	}

	void release_children()
	{
		child0.reset();
		child1.reset();
	}
};



#endif /* __binary_tree_HPP__ */
