struct SYYStackTrace {
	SYYStackTrace* pNext;
	const char* pName;
	int line;
static SYYStackTrace*	s_pStart;
	SYYStackTrace( const char* _pName, int _line )
	{
		pName = _pName;
		line = _line;
		pNext = s_pStart;
		s_pStart = this;
	} // end constructor

	~SYYStackTrace() {
		s_pStart = pNext;
	} // end destructor
};
struct YYObjectBase;
struct CInstance;
struct IBuffer;
struct RValue;
struct CWeakRef;
struct YYRValue;
struct SWithIterator {
	YYObjectBase* pOriginalSelf;
	YYObjectBase* pOriginalOther;

	YYObjectBase** ppBufferBase;
	YYObjectBase** ppCurrent;

	~SWithIterator(){};
};
typedef void (*GML_Call)(RValue& Result, CInstance* selfinst, CInstance* otherinst, int argc, RValue* arg);
#define FREE_RValue(rvp)     do { RValue *__p = (rvp); if (((__p->kind-1)&(MASK_KIND_RVALUE & ~3))==0) { FREE_RValue__Pre(__p); } __p->flags = 0; __p->kind = VALUE_UNDEFINED; __p->ptr = NULL; } while (0);
enum VARIDs {

};
struct YYVAR {
	const char* pName;
	int val;
};

struct YYVARVar
{
	const char* pName;
	enum VARIDs val;
};

struct YYString {
	const char* pStr;

	YYString( const char* _pMessage ) {}

	~YYString() {}

	// cast operator to char*
	operator char* () {}

	YYString& operator=( const YYString& _s ) {}

	YYString& operator=( const char* _pMessage ) {}

	YYString& operator+=( const YYString& rhs ) {}
};
enum RValueType{
    VALUE_REAL = 0,				// Real value
    VALUE_STRING = 1,			// String value
    VALUE_ARRAY = 2,			// Array value
    VALUE_PTR = 3,				// Ptr value
    VALUE_VEC3 = 4,				// Vec3 (x,y,z) value (withthe RValue)
    VALUE_UNDEFINED = 5,		// Undefined value
    VALUE_OBJECT = 6,			// YYObjectBase* value 
    VALUE_INT32 = 7,			// Int32 value
    VALUE_VEC4 = 8,				// Vec4 (x,y,z,w) value (allocated from pool)
    VALUE_VEC44 = 9,			// Vec44 (matrix) value (allocated from pool)
    VALUE_INT64 = 10,			// Int64 value
    VALUE_ACCESSOR = 11,		// Actually an accessor
    VALUE_NULL = 12,			// JS Null
    VALUE_BOOL = 13,			// Bool value
    VALUE_ITERATOR = 14,		// JS For-Iterator
    VALUE_REF = 15,				// Reference value
    VALUE_UNSET = 0x0ffffff		// Unset value (never initialized)
};

const int YYVAR_ACCESSOR_GET = (0);
const int YYVAR_ACCESSOR_SET = (1);

struct RValue;
struct DynamicArrayOfRValue {
	int length;
	RValue* arr;
};

struct RefDynamicArrayOfRValue {
	int	refcount;
	DynamicArrayOfRValue* pArray;
	void* pOwner;
	int visited;
	int length;
};

struct vec3 {
	float	x,y,z;
};

struct vec4 {
	float	x,y,z,w;
};

struct matrix44 {
	vec4	m[4];
};
#pragma pack(push, 4)
struct RValue
{
	union {
		int v32;
		long long v64;
		double	val;						// value when real
		union {
			union {
				void* pRefString;
				//char*	str;						// value when string
				RefDynamicArrayOfRValue* pRefArray;	// pointer to the array
				vec4* pVec4;
				matrix44* pMatrix44;
				void* ptr;
				YYObjectBase* pObj;
				//vec3 v3;
			};
			/*struct {
				float x, y, z;
			} v3;*/
            //float v3[3];
		};
	};
	int		flags;							// use for flags (Hijack for Enumerable and Configurable bits JavaScript)  (Note: probably will need a visibility as well, to support private variables that are promoted to object scope, but should not be seen (is that just not enumerated????) )
	RValueType		kind;							// kind of value

	void Serialise(IBuffer* _buffer);
	void DeSerialise(IBuffer* _buffer);

	const char* GetString() const {}
	long long asInt64() const {}
	double asReal() const {}
};

// new structure used to initialise constant numbers at global scope (to eliminate construction overhead).
struct DValue{
	double	val;
	int		dummy;
	int		kind;
};

struct DLValue{
	long long	val;
	int		dummy;
	int		kind;
};
#pragma pack(pop)

struct CInstanceBase {
	YYRValue*		yyvars;
	virtual ~CInstanceBase() {};
	virtual  YYRValue& GetYYVarRef(VARIDs index)=0;
};

struct YYRValue : RValue{};
typedef	void (*PFUNC_YYGML)( CInstance* _pSelf, CInstance* _pOther );
typedef	void (*PFUNC_YYGMLScript)( CInstance* _pSelf, CInstance* _pOther, RValue& _result, int _count, YYRValue* _args[] );
struct YYGMLFuncs{
	const char* pName;
    union{
        PFUNC_YYGML pFuncObject;
        PFUNC_YYGMLScript pFuncScript;
    };
};
struct SLLVMVars {
	char*			pWad;				// pointer to the Wad
	int				nWadFileLength;		// the length of the wad
	int				nGlobalVariables;	// global varables
	int				nInstanceVariables;	// instance variables
	int				nYYCode;
	YYVAR**			ppVars;
	YYVAR**			ppFuncs;
	YYGMLFuncs*		pGMLFuncs;
	void*			pYYStackTrace;		// pointer to the stack trace
};
typedef void (*PFUNC_InitYYC)( SLLVMVars* _pVars );
struct RToken {
    int m_Kind;
    unsigned int m_Type;
    int m_Ind;
    int m_Ind2;
    RValue m_Value;
    int m_ItemNumber;
    RToken* m_Items;
    int m_Position;
};
using CHashMapHash = unsigned int;

template <typename TKey, typename TValue>
struct CHashMapElement {
	TValue m_Value;
	TKey m_Key;
	CHashMapHash m_Hash;
};

template <typename TKey, typename TValue, int TInitialMask>
struct CHashMap {
	int m_CurrentSize;
	int m_UsedCount;
	int m_CurrentMask;
	int m_GrowThreshold;
	CHashMapElement<TKey, TValue>* m_Elements;
	void(*m_DeleteValue)(TKey* Key, TValue* Value);
};

// Newer struct, later renamed to LinkedList - OLinkedList is used in older x86 games, 
// and causes misalingment due to alignment changing from 8-bytes in x64 to 4-bytes in x86.
template <typename T>
struct LinkedList
{
	T* m_First;
	T* m_Last;
	int m_Count;
	int m_DeleteType;
};

template <typename T>
struct OLinkedList
{
	T* m_First;
	T* m_Last;
	int m_Count;
};

struct CCode {
    int (**_vptr$CCode)(void);
    CCode* m_Next;
    int m_Kind;
    int m_Compiled;
    const char* m_Str;
    RToken m_Token;
    RValue m_Value;
    void* m_VmInstance;
    void* m_VmDebugInfo;
    char* m_Code;
    const char* m_Name;
    int m_CodeIndex;
    YYGMLFuncs* m_Functions;
    bool m_Watch;
    int m_Offset;
    int m_LocalsCount;
    int m_ArgsCount;
    int m_Flags;
    YYObjectBase* m_Prototype;

    const char* GetName() const { return this->m_Name; }
};

using FNGetVariable = bool(*)(
	CInstance* Instance,
	int Index, 
	RValue* Value
);

using FNSetVariable = bool(*)(
	CInstance* Instance,
	int Index, 
	RValue* Value
);

struct RVariableRoutine {
	const char* m_Name;
	FNGetVariable m_GetVariable;
	FNSetVariable m_SetVariable;
	bool m_CanBeSet;
};

enum EJSRetValBool : int{
	EJSRVB_FALSE,
	EJSRVB_TRUE,
	EJSRVB_TYPE_ERROR
};

typedef void(*FNGetOwnProperty)(
		YYObjectBase* Object,
		RValue& Result,
		const char* Name
		);

typedef void(*FNDeleteProperty)(
	YYObjectBase* Object,
	RValue& Result,
	const char* Name,
	bool ThrowOnError
	);

typedef EJSRetValBool(*FNDefineOwnProperty)(
	YYObjectBase* Object,
	const char* Name,
	RValue& Result,
	bool ThrowOnError
	);

enum YYObjectKind : int{
	OBJECT_KIND_YYOBJECTBASE = 0,
	OBJECT_KIND_CINSTANCE = 1,
	OBJECT_KIND_ACCESSOR = 2,
	OBJECT_KIND_SCRIPTREF = 3,
	OBJECT_KIND_PROPERTY,
	OBJECT_KIND_ARRAY,
	OBJECT_KIND_WEAKREF,
	OBJECT_KIND_CONTAINER,
	OBJECT_KIND_SEQUENCE,
	OBJECT_KIND_SEQUENCEINSTANCE,
	OBJECT_KIND_SEQUENCETRACK,
	OBJECT_KIND_SEQUENCECURVE,
	OBJECT_KIND_SEQUENCECURVECHANNEL,
	OBJECT_KIND_SEQUENCECURVEPOINT,
	OBJECT_KIND_SEQUENCEKEYFRAMESTORE,
	OBJECT_KIND_SEQUENCEKEYFRAME,
	OBJECT_KIND_SEQUENCEKEYFRAMEDATA,
	OBJECT_KIND_SEQUENCEEVALTREE,
	OBJECT_KIND_SEQUENCEEVALNODE,
	OBJECT_KIND_SEQUENCEEVENT,
	OBJECT_KIND_NINESLICE,
	OBJECT_KIND_FILTERHOST,
	OBJECT_KIND_EFFECTINSTANCE,
	OBJECT_KIND_SKELETON_SKIN,
	OBJECT_KIND_AUDIOBUS,
	OBJECT_KIND_AUDIOEFFECT,
	OBJECT_KIND_MAX
};

struct YYObjectBase : CInstanceBase{
	virtual RValue& InternalGetYYVarRef(
		VARIDs Index
	) = 0;

	virtual RValue& InternalGetYYVarRefL(
		VARIDs Index
	) = 0;

	virtual bool Mark4GC(
		unsigned int*,
		int
	) = 0;

	virtual bool MarkThisOnly4GC(
		unsigned int*,
		int
	) = 0;

	virtual bool MarkOnlyChildren4GC(
		unsigned int*,
		int
	) = 0;

	virtual void Free(
		bool preserve_map
	) = 0;

	virtual void ThreadFree(
		bool preserve_map,
		void* GCContext
	) = 0;

	virtual void PreFree() = 0;

	virtual RValue* GetDispose() = 0;

	bool Add(
		const char* Name,
		const RValue& Value,
		int Flags
	);

	bool IsExtensible();

	RValue* FindOrAllocValue(
		const char* Name
	);

	YYObjectBase* m_Flink;
	YYObjectBase* m_Blink;
	YYObjectBase* m_Prototype;
	const char* m_Class;
	FNGetOwnProperty m_GetOwnProperty;
	FNDeleteProperty m_DeleteProperty;
	FNDefineOwnProperty m_DefineOwnProperty;
	// Use GetInstanceMember instead
	CHashMap<int, RValue*, 3>* m_YYVarsMap;
	CWeakRef** m_WeakRef;
	unsigned int m_WeakRefCount;
	unsigned int m_VariableCount;
	unsigned int m_Flags;
	unsigned int m_Capacity;
	unsigned int m_Visited;
	unsigned int m_VisitedGC;
	int m_GCGeneration;
	int m_GCCreationFrame;
	int m_Slot;
	YYObjectKind m_ObjectKind;
	int m_RValueInitType;
	int m_CurrentSlot;
};
struct CInstance : YYObjectBase {};