/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: T12-Function.cpp
@Time: 2022/1/23 3:22 下午
@Desc: 本例主要展示，通过GFunction实现节点和函数式混合编程的功能
***************************/

#include "MyGNode/MyNode1.h"
#include "MyGNode/MyWriteParamNode.h"

using namespace CGraph;

void tutorial_function() {
    GPipelinePtr pipeline = GPipelineFactory::create();
    GElementPtr a, b, c_function, d_function = nullptr;

    /** v1.8.3版本之后，推荐使用+=的形式，对status赋值 */
    CStatus status = pipeline->registerGElement<MyNode1>(&a, {}, "nodeA", 1);
    status += pipeline->registerGElement<MyWriteParamNode>(&b, {a}, "nodeB", 1);
    status += pipeline->registerGElement<GFunction>(&c_function, {b}, "functionC", 1);    // 注册GFunction类型的节点c_function
    status += pipeline->registerGElement<GFunction>(&d_function, {c_function}, "functionD", 3);
    if (!status.isOK()) {
        CGRAPH_ECHO("pipeline register failed for the reason : ", status.getInfo().c_str());
        return;
    }

    int num = 10;
    const std::string& info = "Hello, CGraph";
    ((GFunctionPtr)c_function)->setFunction(GFunctionType::RUN, [num, info] {
        CGRAPH_ECHO("input num i = [%d], info = [%s]", num, info.c_str());    // 传递pipeline外部参数，并在pipeline内部节点执行时使用
        return CStatus();
    });

    /** 通过链式调用的方式，注册多个执行函数 */
    ((GFunctionPtr)d_function)->setFunction(GFunctionType::INIT, [d_function] {
        CGRAPH_ECHO("[%s] do init function ...", d_function->getName().c_str());
        return CStatus();
    })->setFunction(GFunctionType::RUN, [d_function, num] {
        auto param = d_function->getGParam<MyParam>("param1");     // 在nodeB(MyWriteParamNode)的init阶段生成，正式使用的时候请注意判空逻辑
        param->iCount += num;
        CGRAPH_ECHO("[%s] do run function, iCount = [%d], iValue = [%d] ...",
                    d_function->getName().c_str(), param->iCount, ++param->iValue);
        return CStatus();
    });

    pipeline->process();
    GPipelineFactory::destroy(pipeline);
}


int main() {
    tutorial_function();
    return 0;
}
