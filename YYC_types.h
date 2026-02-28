struct SYYStackTrace
{
	SYYStackTrace* pNext;
	const char* pName;
	int line;
	static SYYStackTrace*	s_pStart;
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
};
typedef void(*GML_Call)(RValue& Result, CInstance* selfinst, CInstance* otherinst, int argc, RValue* arg);
struct YYVAR {
	const char* pName;
	int val;
};
struct YYString
{
	const char* pStr;
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

struct RValue;
struct DynamicArrayOfRValue
{
	int length;
	RValue* arr;
};

class RefDynamicArrayOfRValue;

struct vec3
{
	float	x, y, z;
};

struct vec4
{
	float	x, y, z, w;
};

struct matrix44
{
	vec4	m[4];
};

#pragma pack( push, 4)
struct RValue
{
	union {
		int v32;
		long long v64;
		double	val;						// value when real
		union {
			union {
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
	unsigned int		flags;							// use for flags (Hijack for Enumerable and Configurable bits in JavaScript)  (Note: probably will need a visibility as well, to support private variables that are promoted to object scope, but should not be seen (is that just not enumerated????) )
	RValueType		kind;							// kind of value
};

struct YYRValue : RValue{};

// new structure used to initialise constant numbers at global scope (to eliminate construction overhead).
struct DValue
{
	double	val;
	int		dummy;
	RValueType		kind;
};

struct DLValue
{
	long long	val;
	int		dummy;
	RValueType		kind;
};

#pragma pack(pop)

class CInstanceBase
{
public:
	virtual ~CInstanceBase() {};
	YYRValue& GetYYVarRef(int index) {
		return InternalGetYYVarRef(index);
	} // end GetYYVarRef
	virtual  YYRValue& InternalGetYYVarRef(int index) = 0;
	YYRValue& GetYYVarRefL(int index) {
		return InternalGetYYVarRefL(index);
	} // end GetYYVarRef
	virtual  YYRValue& InternalGetYYVarRefL(int index) = 0;
};

typedef YYRValue& (*PFUNC_YYGMLScript)( CInstance* pSelf, CInstance* pOther, YYRValue& _result, int _count,  YYRValue** _args  );
typedef	void(*PFUNC_YYGML)(CInstance* _pSelf, CInstance* _pOther);
struct YYGMLFuncs
{
    const char* m_Name;
    union
    {
        PFUNC_YYGMLScript m_ScriptFunction;
        PFUNC_YYGML m_CodeFunction;
    };
    YYVAR* m_FunctionVariables;
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
extern SLLVMVars*	g_pLLVMVars;
typedef void(*PFUNC_InitYYC)(SLLVMVars* _pVars);

using CHashMapHash = unsigned int;

template <typename TKey, typename TValue>
struct CHashMapElement
{
    TValue m_Value;
    TKey m_Key;
    CHashMapHash m_Hash;
};

template <typename TKey, typename TValue, int TInitialMask>
struct CHashMap
{
    int m_CurrentSize;
    int m_UsedCount;
    int m_CurrentMask;
    int m_GrowThreshold;
    CHashMapElement<TKey, TValue>* m_Elements;
    void(*m_DeleteValue)(TKey* Key, TValue* Value);
};

enum EJSRetValBool : int
{
    EJSRVB_FALSE,
    EJSRVB_TRUE,
    EJSRVB_TYPE_ERROR
};

using FNGetOwnProperty = void(*)(
    YYObjectBase* Object,
    RValue& Result,
    const char* Name
    );

using FNDeleteProperty = void(*)(
    YYObjectBase* Object,
    RValue& Result,
    const char* Name,
    bool ThrowOnError
    );

using FNDefineOwnProperty = EJSRetValBool(*)(
    YYObjectBase* Object,
    const char* Name,
    RValue& Result,
    bool ThrowOnError
    );

enum YYObjectKind : int
{
    OBJECT_KIND_YYOBJECTBASE = 0,
    OBJECT_KIND_CINSTANCE,
    OBJECT_KIND_ACCESSOR,
    OBJECT_KIND_SCRIPTREF,
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

struct YYObjectBase : CInstanceBase
{
    virtual YYRValue& InternalGetYYVarRef(
        int Index
    ) = 0;

    virtual YYRValue& InternalGetYYVarRefL(
        int Index
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

struct RToken
{
    int m_Kind;
    unsigned int m_Type;
    int m_Ind;
    int m_Ind2;
    RValue m_Value;
    int m_ItemNumber;
    RToken* m_Items;
    int m_Position;
};

struct CCode
{
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
};
