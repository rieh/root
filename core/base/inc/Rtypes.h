/* @(#)root/base:$Id$ */

/*************************************************************************
 * Copyright (C) 1995-2014, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_Rtypes
#define ROOT_Rtypes

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Rtypes                                                               //
//                                                                      //
// Basic types used by ROOT; ClassDef macros.                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_RtypesCore
#include "RtypesCore.h"
#endif
#ifndef ROOT_DllImport
#include "DllImport.h"
#endif

#include "snprintf.h"   // part of stdio.h on systems that have it
#include "strlcpy.h"    // part of string.h on systems that have it

#include <atomic>
#include <stdio.h>
#include <string.h>
#include <typeinfo>


//---- forward declared class types --------------------------------------------

class TClass;
class TBuffer;
class TDirectory;
class TMemberInspector;
class TObject;
class TNamed;
class TCollection;
class TFileMergeInfo;
class TString;

//Moved from TSystem.
enum ESysConstants {
   kMAXSIGNALS       = 15,
   kMAXPATHLEN       = 8192,
   kBUFFERSIZE       = 8192,
   kItimerResolution = 10      // interval-timer resolution in ms
};

enum EColor { kWhite =0,   kBlack =1,   kGray=920,
              kRed   =632, kGreen =416, kBlue=600, kYellow=400, kMagenta=616, kCyan=432,
              kOrange=800, kSpring=820, kTeal=840, kAzure =860, kViolet =880, kPink=900 };

// There is several streamer concepts.
class TClassStreamer;   // Streamer functor for a class
class TMemberStreamer;  // Streamer functor for a data member
typedef void (*ClassStreamerFunc_t)(TBuffer&, void*);  // Streamer function for a class
typedef void (*ClassConvStreamerFunc_t)(TBuffer&, void*, const TClass*);  // Streamer function for a class with conversion.
typedef void (*MemberStreamerFunc_t)(TBuffer&, void*, Int_t); // Streamer function for a data member

// This class is used to implement proxy around collection classes.
class TVirtualCollectionProxy;

typedef void    (*VoidFuncPtr_t)();  //pointer to void function
typedef TClass* (*DictFuncPtr_t)();  //pointer to dictionary function
// NOTE: the previous name must be changed.

//--- bit manipulation ---------------------------------------------------------

#define BIT(n)       (1ULL << (n))
#define SETBIT(n,i)  ((n) |= BIT(i))
#define CLRBIT(n,i)  ((n) &= ~BIT(i))
#define TESTBIT(n,i) ((Bool_t)(((n) & BIT(i)) != 0))


//---- debug global ------------------------------------------------------------

R__EXTERN Int_t gDebug;


//---- ClassDef macros ---------------------------------------------------------

typedef void (*ShowMembersFunc_t)(const void *obj, TMemberInspector &R__insp, Bool_t isTransient);
class TVirtualIsAProxy;
typedef TClass *(*IsAGlobalFunc_t)(const TClass*, const void *obj);

// TBuffer.h declares and implements the following 2 operators
template <class Tmpl> TBuffer &operator>>(TBuffer &buf, Tmpl *&obj);
template <class Tmpl> TBuffer &operator<<(TBuffer &buf, const Tmpl *obj);

// This might get used if we implement setting a class version.
// template <class RootClass> Short_t GetClassVersion(RootClass *);

namespace ROOT {

   class TGenericClassInfo;
   typedef void *(*NewFunc_t)(void *);
   typedef void *(*NewArrFunc_t)(Long_t size, void *arena);
   typedef void  (*DelFunc_t)(void *);
   typedef void  (*DelArrFunc_t)(void *);
   typedef void  (*DesFunc_t)(void *);
   typedef void  (*DirAutoAdd_t)(void *, TDirectory *);
   typedef Long64_t (*MergeFunc_t)(void *, TCollection *, TFileMergeInfo *);
   typedef void  (*ResetAfterMergeFunc_t)(void *, TFileMergeInfo *);

   template <class RootClass> Short_t SetClassVersion(RootClass *);

   extern TClass *CreateClass(const char *cname, Version_t id,
                              const std::type_info &info, TVirtualIsAProxy *isa,
                              const char *dfil, const char *ifil,
                              Int_t dl, Int_t il);
   extern void AddClass(const char *cname, Version_t id, const std::type_info &info,
                        DictFuncPtr_t dict, Int_t pragmabits);
   extern void RemoveClass(const char *cname);
   extern void ResetClassVersion(TClass*, const char*, Short_t);
   extern void AddClassAlternate(const char *normName, const char *alternate);

   extern TNamed *RegisterClassTemplate(const char *name,
                                        const char *file, Int_t line);

   extern void Class_ShowMembers(TClass *cl, const void *obj, TMemberInspector&);

#if 0
   // This function is only implemented in the dictionary file.
   // The parameter is 'only' for overloading resolution.
   // Used to be a template <class T> TGenericClassInfo *GenerateInitInstance(const T*);
   template <class T> TGenericClassInfo *GetClassInfo(const T* t) {
      TGenericClassInfo *GenerateInitInstance(const T*);
      return CreateInitInstance(t);
   };
#endif

   namespace Internal {
   class TInitBehavior {
      // This class defines the interface for the class registration and
      // the TClass creation. To modify the default behavior, one would
      // inherit from this class and overload ROOT::DefineBehavior().
      // See TQObject.h and table/inc/Ttypes.h for examples.
   public:
      virtual ~TInitBehavior() { }

      virtual void Register(const char *cname, Version_t id,
                            const std::type_info &info,
                            DictFuncPtr_t dict, Int_t pragmabits) const = 0;
      virtual void Unregister(const char *classname) const = 0;
      virtual TClass *CreateClass(const char *cname, Version_t id,
                                  const std::type_info &info, TVirtualIsAProxy *isa,
                                  const char *dfil, const char *ifil,
                                  Int_t dl, Int_t il) const = 0;
   };

   class TDefaultInitBehavior: public TInitBehavior {
   public:
      virtual void Register(const char *cname, Version_t id,
                            const std::type_info &info,
                            DictFuncPtr_t dict, Int_t pragmabits) const {
         ROOT::AddClass(cname, id, info, dict, pragmabits);
      }

      virtual void Unregister(const char *classname) const {
         ROOT::RemoveClass(classname);
      }

      virtual TClass *CreateClass(const char *cname, Version_t id,
                                  const std::type_info &info, TVirtualIsAProxy *isa,
                                  const char *dfil, const char *ifil,
                                  Int_t dl, Int_t il) const {
         return ROOT::CreateClass(cname, id, info, isa, dfil, ifil, dl, il);
      }
   };

   const TInitBehavior *DefineBehavior(void * /*parent_type*/,
                                       void * /*actual_type*/);
   } // namespace Internal

} // namespace ROOT

// The macros below use TGenericClassInfo and TInstrumentedIsAProxy, so let's
// ensure they are included.
#ifndef ROOT_TGenericClassInfo
#include "TGenericClassInfo.h"
#endif
#ifndef ROOT_TIsAProxy
#include "TIsAProxy.h"
#endif


// Parts or ROOT are compiled with no-rtti and cannot use or even see typeid().
// Hide the following declaration (that is only needed by ClassDefInline) for
// these cases by #defining R__NO_INLINE_CLASSDEF:
#ifndef R__NO_INLINE_CLASSDEF
namespace ROOT {
   template <typename T>
   class ClassDefGenerateInitInstanceLocalInjector {
      static TClass* fgIsA;
   public:
      static void *New(void *p) { return p ? new(p) T : new T; };
      static void *NewArray(Long_t nElements, void *p) {
         return p ? new(p) T[nElements] : new T[nElements]; }
      static void Delete(void *p) { delete ((T*)p); }
      static void DeleteArray(void *p) { delete[] ((T*)p); }
      static void Destruct(void *p) { ((T*)p)->~T();  }
      static ::ROOT::TGenericClassInfo *GenerateInitInstanceLocal() {
         T *ptr = 0;
         static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy<T>(0);
         static ::ROOT::TGenericClassInfo
            R__instance(T::Class_Name(), T::Class_Version(),
                        T::DeclFileName(), T::DeclFileLine(),
                        typeid(T), ROOT::Internal::DefineBehavior(ptr, ptr),
                        &T::Dictionary, isa_proxy, 0, sizeof(T) );
         R__instance.SetNew(&New);
         R__instance.SetNewArray(&NewArray);
         R__instance.SetDelete(&Delete);
         R__instance.SetDeleteArray(&DeleteArray);
         R__instance.SetDestructor(&Destruct);
         return &R__instance;
      }
      static void Dictionary() { fgIsA = GenerateInitInstanceLocal()->GetClass(); }
      static TClass *Class() { if (!fgIsA) Dictionary(); return fgIsA; }
   };

   template<typename T>
   TClass* ClassDefGenerateInitInstanceLocalInjector<T>::fgIsA = 0;
} // namespace ROOT
#endif // R__NO_INLINE_CLASSDEF


typedef std::atomic<TClass*> atomic_TClass_ptr;

// Common part of ClassDef definition.
// DeclFileLine() is not part of it since CINT uses that as trigger for
// the class comment string.
#define _ClassDefBase_(name,id, virtual_keyword, overrd) \
public: \
   static Version_t Class_Version() { return id; } \
   virtual_keyword TClass *IsA() const overrd { return name::Class(); } \
   virtual_keyword void ShowMembers(TMemberInspector&insp) const overrd { ::ROOT::Class_ShowMembers(name::Class(), this, insp); } \
   void StreamerNVirtual(TBuffer&ClassDef_StreamerNVirtual_b) { name::Streamer(ClassDef_StreamerNVirtual_b); } \
   static const char *DeclFileName() { return __FILE__; }


#define _ClassDefOutline_(name,id, virtual_keyword, overrd) \
   _ClassDefBase_(name,id, virtual_keyword, overrd)       \
private: \
   static atomic_TClass_ptr fgIsA; \
public: \
   static int ImplFileLine(); \
   static const char *ImplFileName(); \
   static const char *Class_Name(); \
   static TClass *Dictionary(); \
   static TClass *Class(); \
   virtual_keyword void Streamer(TBuffer&) overrd;

#define _ClassDefInline_(name,id, virtual_keyword, overrd) \
   _ClassDefBase_(name,id, virtual_keyword, overrd)       \
public: \
   static int ImplFileLine() { return -1; }     \
   static const char *ImplFileName() { return 0; }      \
   static const char *Class_Name() { return #name; } \
   static void Dictionary() { ROOT::ClassDefGenerateInitInstanceLocalInjector< name >::Dictionary(); } \
   static TClass *Class() { return ROOT::ClassDefGenerateInitInstanceLocalInjector< name >::Class(); } \
   virtual_keyword void Streamer(TBuffer& R__b) overrd {                \
      if (R__b.IsReading()) R__b.ReadClassBuffer(name::Class(),this);   \
      else R__b.WriteClassBuffer(name::Class(),this);}

#define ClassDef(name,id) \
   _ClassDefOutline_(name,id,virtual,)   \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefOverride(name,id) \
   _ClassDefOutline_(name,id,,override)   \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefNV(name,id) \
   _ClassDefOutline_(name,id,,) \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefInline(name,id) \
   _ClassDefInline_(name,id,virtual,)                   \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefInlineOverride(name,id)                       \
   _ClassDefInline_(name,id,,override)                        \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefInlineNV(name,id) \
   _ClassDefInline_(name,id,,)                         \
   static int DeclFileLine() { return __LINE__; }

//#define _ClassDefInterp_(name,id) ClassDefInline(name,id)

#define R__UseDummy(name) \
   class _NAME2_(name,_c) { public: _NAME2_(name,_c)() { if (name) { } } }


#define ClassImpUnique(name,key) \
   namespace ROOT { \
      TGenericClassInfo *GenerateInitInstance(const name*); \
      namespace { \
         static int _R__UNIQUE_(_NAME2_(R__dummyint,key)) = \
            GenerateInitInstance((name*)0x0)->SetImplFile(__FILE__, __LINE__); \
         R__UseDummy(_R__UNIQUE_(_NAME2_(R__dummyint,key))); \
      } \
   }
#define ClassImp(name) ClassImpUnique(name,default)

// Macro for Namespace

#define NamespaceImpUnique(name,key) \
   namespace name { \
      namespace ROOT { \
         ::ROOT::TGenericClassInfo *GenerateInitInstance(); \
         namespace { \
            static int _R__UNIQUE_(_NAME2_(R__dummyint,key)) = \
               GenerateInitInstance()->SetImplFile(__FILE__, __LINE__); \
            R__UseDummy(_R__UNIQUE_(_NAME2_(R__dummyint,key))); \
         } \
      } \
   }
#define NamespaceImp(name) NamespaceImpUnique(name,default)

//---- ClassDefT macros for templates with one template argument ---------------
// ClassDefT  corresponds to ClassDef
// ClassDefT2 goes in the same header as ClassDefT but must be
//            outside the class scope
// ClassImpT  corresponds to ClassImp


// This ClassDefT is stricly redundant and is kept only for
// backward compatibility.

#define ClassDefT(name,id) \
   _ClassDefOutline_(name,id,virtual,) \
   static int DeclFileLine() { return __LINE__; }

#define ClassDefTNV(name,id) \
   _ClassDefOutline_(name,id,virtual,) \
   static int DeclFileLine() { return __LINE__; }


#define ClassDefT2(name,Tmpl)



#define templateClassImpUnique(name,key) \
   namespace ROOT { \
      static TNamed *_R__UNIQUE_(_NAME2_(R__dummyholder,key)) = \
         ROOT::RegisterClassTemplate(_QUOTE_(name), __FILE__, __LINE__); \
      R__UseDummy(_R__UNIQUE_(_NAME2_(R__dummyholder,key))); \
   }
#define templateClassImp(name) templateClassImpUnique(name,default)

#define ClassImpT(name,Tmpl) templateClassImp(name)

//---- ClassDefT macros for templates with two template arguments --------------
// ClassDef2T2 goes in the same header as ClassDefT but must be
//             outside the class scope
// ClassImp2T  corresponds to ClassImpT

#define ClassDef2T2(name,Tmpl1,Tmpl2)
#define ClassImp2T(name,Tmpl1,Tmpl2) templateClassImp(name)


//---- ClassDefT macros for templates with three template arguments ------------
// ClassDef3T2 goes in the same header as ClassDefT but must be
//             outside the class scope
// ClassImp3T  corresponds to ClassImpT

#define ClassDef3T2(name,Tmpl1,Tmpl2,Tmpl3)
#define ClassImp3T(name,Tmpl1,Tmpl2,Tmpl3) templateClassImp(name)


//---- Macro to set the class version of non instrumented classes --------------

#define RootClassVersion(name,VersionNumber) \
namespace ROOT { \
   TGenericClassInfo *GenerateInitInstance(const name*); \
   static Short_t _R__UNIQUE_(R__dummyVersionNumber) = \
           GenerateInitInstance((name*)0x0)->SetVersion(VersionNumber); \
   R__UseDummy(_R__UNIQUE_(R__dummyVersionNumber)); \
}

#define RootStreamer(name,STREAMER)                                  \
namespace ROOT {                                                     \
   TGenericClassInfo *GenerateInitInstance(const name*);             \
   static Short_t _R__UNIQUE_(R__dummyStreamer) =                    \
           GenerateInitInstance((name*)0x0)->SetStreamer(STREAMER);  \
   R__UseDummy(_R__UNIQUE_(R__dummyStreamer));                       \
}

//---- Macro to load a library into the interpreter --------------
// Call as R__LOAD_LIBRARY(libEvent)
// This macro intentionally does not take string as argument, to
// prevent compilation errors with complex diagnostics due to
//   TString BAD_DO_NOT_TRY = "lib";
//   R__LOAD_LIBRARY(BAD_DO_NOT_TRY + "BAD_DO_NOT_TRY.so") // ERROR!
#define _R_PragmaStr(x) _Pragma(#x)
#ifdef __CLING__
# define R__LOAD_LIBRARY(LIBRARY) _R_PragmaStr(cling load ( #LIBRARY ))
# define R__ADD_INCLUDE_PATH(PATH) _R_PragmaStr(cling add_include_path ( #PATH ))
# define R__ADD_LIBRARY_PATH(PATH) _R_PragmaStr(cling add_library_path ( #PATH ))
#elif defined(R__WIN32)
# define R__LOAD_LIBRARY(LIBRARY) _R_PragmaStr(comment(lib, #LIBRARY))
# define R__ADD_INCLUDE_PATH(PATH) _R_PragmaStr(comment(path, #PATH))
# define R__ADD_LIBRARY_PATH(PATH) _R_PragmaStr(comment(path, #PATH))
#else
// No way to inform linker though preprocessor :-(
// We could even inform the user:
/*
# define R__LOAD_LIBRARY(LIBRARY) \
   _R_PragmaStr(message "Compiler cannot handle linking against " #LIBRARY \
                ". Use -L and -l instead.")
*/
# define R__LOAD_LIBRARY(LIBRARY)
# define R__ADD_INCLUDE_PATH(PATH)
# define R__ADD_LIBRARY_PATH(PATH)
#endif

#endif
