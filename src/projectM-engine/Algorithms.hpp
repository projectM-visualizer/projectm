#ifndef PROJECTM_ALGORITHMS_HPP
#define PROJECTM_ALGORITHMS_HPP
#include <cassert>

namespace Algorithms
{

  template <class TraverseFunctor, class Container>
  void traverse(Container & container)
  {

    TraverseFunctor functor;

    for (typename Container::iterator pos = container.begin(); pos != container.end(); ++pos)
    {
      functor(pos->second);
    }

  }


  template <class TraverseFunctor, class Container>
  void traverse(Container & container, TraverseFunctor & functor)
  {

    for (typename Container::iterator pos = container.begin(); pos != container.end(); ++pos)
    {
      functor(pos->second);
    }

  }

  namespace TraverseFunctors
  {
    template <class Data>
    class DeleteFunctor
    {

    public:

      void operator() (Data * data)
      {
        assert(data);
        delete(data);
      }

    };
  }


}
#endif
