#ifndef NULLSOFT_VECTOR_H
#define NULLSOFT_VECTOR_H
#include <assert.h>
#include <bfc/platform/types.h>

template <class Type, int INCREMENT = 32, int MULTIPLIER=1>
class Vector
{
public:
	typedef Type *iterator;
	typedef const Type *const_iterator;
public:
	Vector() : values(0), numPtrs(0), allocSize(0)
	{}
	virtual ~Vector()
	{
		delete [] values;
	}

	Vector(const Vector<Type, INCREMENT, MULTIPLIER> &copy): values(0), numPtrs(0), allocSize(0)
	{
		if (copy.numPtrs)
		{
			values = new Type[copy.numPtrs];
			allocSize = copy.numPtrs;
			numPtrs = copy.numPtrs;
			for (size_t i = 0;i != numPtrs;i++)
			{
				values[i] = copy.values[i];
			}
		}
	}

	void operator=(const Vector<Type, INCREMENT, MULTIPLIER> &copy)
	{
		Reset();
		if (copy.numPtrs)
		{
			values = new Type[copy.numPtrs];
			allocSize = copy.numPtrs;
			numPtrs = copy.numPtrs;
			for (size_t i = 0;i != numPtrs;i++)
			{
				values[i] = copy.values[i];
			}
		}
	}

	Type &operator[](size_t index)
	{
		return values[index];
	}
	Type *data()
	{
		return values;
	}
	Type *begin() const
	{
		return values;
	}
	Type *end() const
	{
		if (values) return values + numPtrs; else return 0;
	}
	void Reset()
	{
		delete [] values; values = 0; numPtrs = 0; allocSize=0;
	}
	void clear()
	{
		numPtrs = 0;
	}
	size_t size() const
	{
		return numPtrs;
	}
	size_t capacity()
	{
		return allocSize;
	}
	Type &back()
	{
		return values[numPtrs-1];
	}

	Type &at(size_t index) const
	{
		return values[index];
	}

	void reserve(size_t size)
	{
		if (size <= numPtrs)
			return;

		Type *newTable = new Type[size];
		for (size_t i = 0;i != numPtrs;i++)
		{
			newTable[i] = values[i];
		}
		allocSize = size;
		delete[] values;
		values = newTable;
	}

	void push_back(Type t)
	{
		if (numPtrs == allocSize)
			reserve(allocSize*MULTIPLIER + INCREMENT);
		values[numPtrs++] = t;
	}

	void insert(size_t index, const Type &value)
	{
		if (numPtrs == allocSize)
			reserve(allocSize*MULTIPLIER + INCREMENT);

		for (size_t i = numPtrs;i != index;i--)
		{
			values[i] = values[i-1];
		}
		values[index] = value;
		numPtrs++;
	}

	void append(size_t size, Type *t)
	{
		reserve(numPtrs + size + INCREMENT);
		for (size_t i = 0;i != size;i++)
		{
			push_back(t[i]);
		}
	}

	void pop_back()
	{
		if (numPtrs)
		{
			numPtrs--;
			// next line removed to allow structs and classes
			// values[numPtrs] = 0; // TODO: an inplace delete might be better?
		}
	}

	void erase(iterator itr)
	{
		size_t index = itr - values;
		eraseAt(index);
	}

	void eraseAt(size_t index)
	{	
		if (numPtrs > index)
		{
			for (size_t k = index + 1; k < numPtrs; k++) 
			{
				values[k-1] = values[k];
			}
			numPtrs--;
		}
	}

	/* Removes an item by swapping it with the last item in the list.  faster but can ruin order */
	void eraseAtFast(size_t index)
	{
		if (index < numPtrs)
		{
			values[index] = values[--numPtrs];
//			if (numPtrs != index)
//				values[numPtrs]=0;			
		}
	}

	bool empty() const
	{
		return numPtrs == 0;
	}

	void resize(size_t newSize, Type val)
	{
		if (newSize < numPtrs)
		{
			numPtrs = newSize;
		}
		else if (newSize > numPtrs)
		{
			reserve(allocSize + (newSize - numPtrs) + INCREMENT);
			
			while(numPtrs < newSize)
			{
				values[numPtrs] = val;
				numPtrs++;
			}
		}
	}

	void resize(size_t newSize)
	{
		if (newSize < numPtrs)
		{
			numPtrs = newSize;
		}
		else if (newSize > numPtrs)
		{
			reserve(allocSize + (newSize - numPtrs) + INCREMENT);
			numPtrs = newSize;
		}
	}

	void set(Type *ptr, size_t num)
	{
		delete [] values;
		values=ptr;
		numPtrs=num;
	}

private:
	size_t numPtrs;
	size_t allocSize;
	Type *values;
};

#endif
