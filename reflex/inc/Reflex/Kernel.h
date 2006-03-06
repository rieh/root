// @(#)root/reflex:$Name:  $:$Id: Kernel.h,v 1.8 2005/12/12 15:52:12 roiser Exp $
// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2005, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#ifndef ROOT_Reflex_Kernel
#define ROOT_Reflex_Kernel

// These macros will allow selection on exported symbols
// taken from http://www.nedprod.com/programs/gccvisibility.html
// Shared library support
#ifdef WIN32
  #define RFLX_IMPORT __declspec(dllimport)
  #define RFLX_EXPORT __declspec(dllexport)
  #define RFLX_DLLLOCAL
  #define RFLX_DLLPUBLIC
#else
  #define RFLX_IMPORT
  #ifdef GCC_HASCLASSVISIBILITY
    #define RFLX_EXPORT __attribute__ ((visibility("default")))
    #define RFLX_DLLLOCAL __attribute__ ((visibility("hidden")))
    #define RFLX_DLLPUBLIC __attribute__ ((visibility("default")))
  #else
    #define RFLX_EXPORT
    #define RFLX_DLLLOCAL
    #define RFLX_DLLPUBLIC
  #endif
#endif

// Define RFLX_API for DLL builds
#ifdef FOXDLL
  #ifdef FOXDLL_EXPORTS
    #define RFLX_API RFLX_EXPORT
  #else
    #define RFLX_API  RFLX_IMPORT
  #endif // FOXDLL_EXPORTS
#else
  #define RFLX_API
#endif // FOXDLL

// Throwable classes must always be visible on GCC in all binaries
#ifdef WIN32
  #define RFLX_EXCEPTIONAPI(api) api
#elif defined(GCC_HASCLASSVISIBILITY)
  #define RFLX_EXCEPTIONAPI(api) RFLX_EXPORT
#else
  #define RFLX_EXCEPTIONAPI(api)
#endif
// end macros for symbol selection


// include config.h generated by autoconf
#if defined (HAVE_CONFIG)
#include "config.h"
#endif

#if defined (_AIX)
#define ANSICPP
#define NEED_STRCASECMP
#endif

#if defined(__alpha) && !defined(__linux)
#ifndef __USE_STD_IOSTREAM
#define __USE_STD_IOSTREAM
#endif
#endif

// Some pragmas to avoid warnings in VisualC
#ifdef _WIN32
// windows.h defines CONST
#ifdef CONST
#undef CONST
#endif
// Disable warning C4786: identifier was truncated to '255' characters in the debug information
#pragma warning ( disable : 4786 )
// Disable warning C4291: no matching operator delete found; memory will not be freed if 
// initialization throws an exception
#pragma warning ( disable : 4291 )
// Disable warning C4250: inheritance via dominance
#pragma warning ( disable : 4250 )
#endif

// some compilers define the macros below in limits
#include <limits>

// Large integer definition depends of the platform
#ifdef _WIN32
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
#elif defined(__linux) || defined(sun) || defined(__APPLE__) || (defined(__CYGWIN__)&&defined(__GNUC__)) || defined(_AIX) || (defined(__alpha)&&!defined(__linux)) || defined(__sgi)
typedef long long int longlong; /* */
typedef unsigned long long int /**/ ulonglong;
#endif
#ifndef LONGLONG_MAX
#define LONGLONG_MAX 0x7FFFFFFFFFFFFFFFLL
#endif
#ifndef LONGLONG_MIN
#define LONGLONG_MIN 0x8000000000000000LL
#endif
#ifndef ULONGLONG_MAX
#define ULONGLONG_MAX 0xFFFFFFFFFFFFFFFFLL
#endif
#ifndef ULONGLONG_MIN
#define ULONGLONG_MIN 0x0000000000000000LL
#endif


// Include files
#include <string>
#include <vector>
#include <utility>
#include <exception>

namespace ROOT {
   namespace Reflex {

      // forward declarations
      class Type;
      class Base;
      class Scope;
      class Object;
      class Member;
      class PropertyList;
      class TypeTemplate;
      class MemberTemplate;

      typedef std::vector<std::string> StdString_Cont_Type_t;
      typedef StdString_Cont_Type_t::iterator StdString_Iterator;
      typedef StdString_Cont_Type_t::reverse_iterator Reverse_StdString_Iterator;

      typedef std::vector< Type > Type_Cont_Type_t;
      typedef Type_Cont_Type_t::iterator Type_Iterator;
      typedef Type_Cont_Type_t::reverse_iterator Reverse_Type_Iterator;

      typedef std::vector< Base > Base_Cont_Type_t;
      typedef Base_Cont_Type_t::iterator Base_Iterator;
      typedef Base_Cont_Type_t::reverse_iterator Reverse_Base_Iterator;

      typedef std::vector< Scope > Scope_Cont_Type_t;
      typedef Scope_Cont_Type_t::iterator Scope_Iterator;
      typedef Scope_Cont_Type_t::reverse_iterator Reverse_Scope_Iterator;

      typedef std::vector< Object > Object_Cont_Type_t;
      typedef Object_Cont_Type_t::iterator Object_Iterator;
      typedef Object_Cont_Type_t::reverse_iterator Reverse_Object_Iterator;

      typedef std::vector< Member > Member_Cont_Type_t;
      typedef Member_Cont_Type_t::iterator Member_Iterator;
      typedef Member_Cont_Type_t::reverse_iterator Reverse_Member_Iterator;

      typedef std::vector< TypeTemplate > TypeTemplate_Cont_Type_t;
      typedef TypeTemplate_Cont_Type_t::iterator TypeTemplate_Iterator;
      typedef TypeTemplate_Cont_Type_t::reverse_iterator Reverse_TypeTemplate_Iterator;

      typedef std::vector< MemberTemplate > MemberTemplate_Cont_Type_t;
      typedef MemberTemplate_Cont_Type_t::iterator MemberTemplate_Iterator;
      typedef MemberTemplate_Cont_Type_t::reverse_iterator Reverse_MemberTemplate_Iterator;

      /** some general information about the Reflex package */
      class Reflex {
         public:
      
         /** default constructor */
         Reflex();
      
         /** the Name of the package - used for messages */
         static const std::string & Argv0(); // returns "Seal Reflex";

      }; // struct Reflex

    
      // these defines are used for the modifiers they are used in the following 
      // classes
      // BA = BASE
      // CL = CLASS
      // FU = FUNCTION
      // DM = DATAMEMBER
      // FM = FUNCTIONMEMBER 
      // TY = TYPE
      // ME = MEMBER
      //                              BA  CL  DM  FM  TY  ME 
      enum {
         PUBLIC          = (1<<0),  //  X       X   X       X
         PROTECTED       = (1<<1),  //  X       X   X       X
         PRIVATE         = (1<<2),  //  X       X   X       X
         REGISTER        = (1<<3),  //          X   X       X
         STATIC          = (1<<4),  //          X   X       X
         CONSTRUCTOR     = (1<<5),  //              X       X
         DESTRUCTOR      = (1<<6) , //              X       X
         EXPLICIT        = (1<<7),  //              X       X
         EXTERN          = (1<<8) , //          X   X       X
         COPYCONSTRUCTOR = (1<<9) , //              X       X
         OPERATOR        = (1<<10), //              X       X
         INLINE          = (1<<11), //              X       X
         CONVERTER       = (1<<12), //              X       X
         AUTO            = (1<<13), //          X           X
         MUTABLE         = (1<<14), //          X           X
         CONST           = (1<<15), //          X       X   X
         VOLATILE        = (1<<16), //          X       X   X
         REFERENCE       = (1<<17), //          X           X
         ABSTRACT        = (1<<18), //      X           X
         VIRTUAL         = (1<<19), //  X   X           X
         TRANSIENT       = (1<<20), //          X           X
         ARTIFICIAL      = (1<<21), //  X   X   X   X   X   X
         // the bits 31 - 28 are reserved for template default arguments 
         TEMPLATEDEFAULTS1  = (0<<31)&(0<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS2  = (0<<31)&(0<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS3  = (0<<31)&(0<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS4  = (0<<31)&(1<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS5  = (0<<31)&(1<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS6  = (0<<31)&(1<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS7  = (0<<31)&(1<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS8  = (1<<31)&(0<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS9  = (1<<31)&(0<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS10 = (1<<31)&(0<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS11 = (1<<31)&(0<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS12 = (1<<31)&(1<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS13 = (1<<31)&(1<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS14 = (1<<31)&(1<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS15 = (1<<31)&(1<<30)&(1<<29)&(1<<28)
      };


      /** enum for printing names */
      enum {
         FINAL     = (1<<0),
         QUALIFIED = (1<<1),
         SCOPED    = (1<<2),
         F         = (1<<4),
         Q         = (1<<5),
         S         = (1<<6)
      };
      

      /** enum containing all possible types and scopes */
      enum TYPE { CLASS = 0,
                  STRUCT,
                  ENUM, 
                  FUNCTION, 
                  ARRAY, 
                  FUNDAMENTAL, 
                  POINTER, 
                  POINTERTOMEMBER, 
                  TYPEDEF, 
                  UNION, 
                  TYPETEMPLATEINSTANCE, 
                  MEMBERTEMPLATEINSTANCE, 
                  NAMESPACE, 
                  DATAMEMBER, 
                  FUNCTIONMEMBER,
                  UNRESOLVED
      };


      /** all possible status codes of the package */
      enum STATUS { SUCCESS = 0,
                    FAILURE
      };
    
      /** the Name of the status code -- don't use this to check */
#define SRSTATUSNAME rstat
      /** the Name of the status code to check for */
#define SRSTATUS Reflex::SRSTATUSNAME
        

      /**
       * typedef for function MemberAt At (necessary for return value of
       * getter function)
       */
      typedef void * (* StubFunction) ( void *, const std::vector < void * > &, void *);

      /** typedef for function for Offset calculation */
      typedef size_t( * OffsetFunction ) ( void * );

      /** dummy types for type_info purposes */
      class NullType {};
      class UnknownType {};
      /** place holders for protected types */
      class ProtectedClass {};
      class ProtectedEnum {};
      class ProtectedStruct {};
      class ProtectedUnion {};
      /** place holders for private types */
      class PrivateClass {};
      class PrivateEnum {};
      class PrivateStruct {};
      class PrivateUnion {};
      /** place holders for unnamed types (also typeinfo purposes) */
      class UnnamedClass {};
      class UnnamedEnum {};
      class UnnamedNamespace {};
      class UnnamedStruct {};
      class UnnamedUnion {};
      
      /** exception classes */
      class RFLX_EXCEPTIONAPI(RFLX_API) RuntimeError : public std::exception {
      public:
         RuntimeError(const std::string& msg) : fMsg(Reflex::Argv0() + ": " + msg) { }
         ~RuntimeError() throw() {}
         virtual const char * What() const throw( ) { return fMsg.c_str();}
         std::string fMsg;
      };

   } // namespace Reflex
} // namespace ROOT

#endif // ROOT_Reflex_Kernel

