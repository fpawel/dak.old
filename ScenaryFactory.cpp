//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "ScenaryFactory.h"
//---------------------------------------------------------------------------
#include "task_.h"
#include "MyNoDestroySingleton.hpp"
#include "MyExcpt.hpp"
//---------------------------------------------------------------------------
#include <sstream>
//---------------------------------------------------------------------------
#include "Loki\Factory.h"
//---------------------------------------------------------------------------
#include "boost\noncopyable.hpp"
#include "boost\shared_ptr.hpp"
#include "AnsiStringUtils_.h"
#include "MyExcpt.hpp"
#include <forms.hpp>
#include "CheckConcTask.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//------------------------------------------------------------------------------
template <typename IdentifierType, class AbstractProduct>
    struct MyFactoryError
    {
        static AbstractProduct* OnUnknownType(const IdentifierType& s)
        {
            //MY_THROW_(MYSPRINTF_("Не корректный идентификатор: %s!", s));
            AbstractProduct* ret = NULL;
            return ret;
        }
    };
//------------------------------------------------------------------------------
template <class ProductT>
MyTransferTask* CreateScenaryActionForFactory()
{
    return new ProductT();
}
//------------------------------------------------------------------------------
class ScenaryFactoryImplT : public boost::noncopyable
{
public:
    explicit ScenaryFactoryImplT();
    MyTransferTask* CreateProduct(const AnsiString& id)
    {
        return factory_.CreateObject(id);
    }
private:
    typedef Loki::Factory<MyTransferTask, AnsiString, MyTransferTask* (*)(), MyFactoryError > FactoryT;
    typedef FactoryT::ProductCreator ProductCreatorT;
    FactoryT factory_;
    std::vector<AnsiString> cpts_, ids_;

    template<class ProductT>
    void RegisterProductType()
    {
        MyTransferTask* obj = new ProductT();
        ProductCreatorT productCreator = &CreateScenaryActionForFactory<ProductT>;
        const AnsiString
            id = obj->ClassId(),
            cpt = obj->What();
        delete obj;
        if( !factory_.Register( id, productCreator) )
            MY_THROW_("Не возможно зарегестировать тип фабричного продукта: "+id);
        cpts_.push_back( cpt );
        ids_.push_back( id );
    }

};
//------------------------------------------------------------------------------
ScenaryFactoryImplT::ScenaryFactoryImplT()
{
    RegisterProductType<ChainedScenaryAction>();
    RegisterProductType<BlowGas>();
    RegisterProductType<DelayTask>();
    RegisterProductType<InitializeTask>();
    RegisterProductType<FinalizeTask>();
    RegisterProductType<ManualTestTask>();
    RegisterProductType<WriteModbusTask>();
    RegisterProductType<GetModbusConcTask>();
    RegisterProductType<GetStendTask>();
    RegisterProductType<FixTestResultTask>();
    RegisterProductType<CheckStabilityTask>();
    RegisterProductType<ManualGetTemperatureTask>();
    RegisterProductType<SendHeatChamSetpointTask>();
    RegisterProductType<CheckDeviceConnectTask>();
    RegisterProductType<MessageBoxTask>();
    RegisterProductType<ReferenceTask>();
    RegisterProductType<ReadKoefsTask>();
    RegisterProductType<WriteKoefsTask>();
    RegisterProductType<SwitchOffPneumoTask>();
    RegisterProductType<AdjustCurrentTask>();
    RegisterProductType<SetPorogsTask>();
    RegisterProductType<AdjustConcTask>();
    RegisterProductType<SelectGasTask>();
    RegisterProductType<FixTemperatureValTask>();
    RegisterProductType<SendHeatChamControlCommandTask>();
    RegisterProductType<TestHartTask>();
    RegisterProductType<WriteKefs1Task>();

}
//------------------------------------------------------------------------------
struct ScenaryActionFactoryTag {};
typedef MyNoDestroySingletonHolder<ScenaryFactoryImplT, ScenaryActionFactoryTag> ScenaryFactory;
//------------------------------------------------------------------------------
MyTransferTask* CreateScenaryAction(const AnsiString& id)
{
    MyTransferTask* pTask = ScenaryFactory::Instance().CreateProduct(id);
    if(!pTask)
        pTask = new ChainedScenaryAction;
    return pTask;
}
//------------------------------------------------------------------------------


