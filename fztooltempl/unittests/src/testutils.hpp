#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <list>

#include <fztooltempl/exception.hpp>
#include <fztooltempl/utils.hpp>
#include <fztooltempl/test.hpp>

class UtilsTest : public test::TestCase<UtilsTest>{

private:

  class Data{

    std::string x;
    std::string y;

  public:
    
    Data(const std::string &x, const std::string &y){
      this->x = x;
      this->y = y;
    }

    std::string getX() const { return x; }
    std::string getY() const { return y; }

    std::string toString() const {
      return std::string("(") + getX() + std::string(",") + getY() + std::string(")");   
    }

  };

  class DataRepresenter : public utils::Converter<Data, std::string>{

  public:
    
    std::string operator()(const Data &data) const{
      return data.toString();
    }

  };

public:
  
  UtilsTest() : test::TestCase<UtilsTest>(){

    addTest(&UtilsTest::testSeparateFromString,"testSeparateFromString");
    addTest(&UtilsTest::testSeparateEmpty,"testSeparateEmpty");
    addTest(&UtilsTest::testSeparateFromData,"testSeparateFromData");

  }

  void testSeparateFromString() throw (exc::ExceptionBase){
    
    std::list<std::string> slist;
    slist.push_back(std::string("eins"));
    slist.push_back(std::string("zwei"));
    slist.push_back(std::string("drei"));

    assertEquals(std::string("eins und zwei und drei"),utils::separate(slist.begin(),slist.end(),std::string(" und ")));
    
  }

  void testSeparateEmpty() throw (exc::ExceptionBase){
    
    std::list<std::string> slist;

    assertEquals(std::string(""),utils::separate(slist.begin(),slist.end(),std::string(" und ")));
    
  }

  void testSeparateFromData() throw (exc::ExceptionBase){
    
    std::list<Data> dlist;

    dlist.push_back(Data("a","b"));
    dlist.push_back(Data("c","d"));

    assertEquals(std::string("(a,b)(c,d)"),utils::separate(dlist.begin(),dlist.end(),std::string(""),DataRepresenter()));
    
  }

};

#endif
