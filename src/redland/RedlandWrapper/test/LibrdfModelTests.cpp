//
// Created by Ciaran on 5/17/2020.
//

#include "gtest/gtest.h"
#include "LibrdfModel.h"

using namespace redland;

class LibrdfModelTests : public ::testing::Test {

public:
    LibrdfStorage storage1 = LibrdfStorage();
    LibrdfStorage storage2 = LibrdfStorage();

    LibrdfModelTests() = default;

    ~LibrdfModelTests() {
        storage1.freeStorage();
        storage2.freeStorage();
    }

};

TEST_F(LibrdfModelTests, TestNew) {
    LibrdfModel model1 = LibrdfModel(storage1.get());
    ASSERT_NE(model1.get(), nullptr);
    model1.freeModel();
}


TEST_F(LibrdfModelTests, TestMoveConstructor) {
    LibrdfModel model1 = LibrdfModel(std::move(storage1.get()));
    auto model1_int_ptr = reinterpret_cast<std::uintptr_t>(model1.get());
    LibrdfModel model2 = std::move(model1);
    auto model2_int_ptr = reinterpret_cast<std::uintptr_t>(model2.get());
    ASSERT_EQ(model1_int_ptr, model2_int_ptr);
    model2.freeModel();
}

TEST_F(LibrdfModelTests, TestMoveAssignment) {
    LibrdfModel model1 = LibrdfModel(storage1.get());
    // get ptr as int to store for comparison later:
    auto model1_int_ptr = reinterpret_cast<std::uintptr_t>(model1.get());
    LibrdfModel model2 = LibrdfModel(storage2.get());
    model2 = std::move(model1);
    auto model2_int_ptr = reinterpret_cast<std::uintptr_t>(model2.get());
    ASSERT_EQ(model1_int_ptr, model2_int_ptr);
    model2.freeModel();
}


TEST_F(LibrdfModelTests, TestAddStatement) {
    auto model1 = LibrdfModel(storage1.get());
    LibrdfStatement statement = LibrdfStatement::fromRawNodePtrs(
            LibrdfNode::fromUriString("subject").get(),
            LibrdfNode::fromUriString("predicate").get(),
            LibrdfNode::fromUriString("resource").get()
    );
    model1.addStatement(statement);
    int expected = 1;
    int actual = model1.size();
    ASSERT_EQ(expected, actual);
    model1.freeModel();
    statement.freeStatement();
}

TEST_F(LibrdfModelTests, TestRemoveStatement) {
    LibrdfModel model1 = LibrdfModel(storage1.get());
    LibrdfStatement statement1 = LibrdfStatement::fromRawNodePtrs(
            LibrdfNode::fromUriString("subject1").get(),
            LibrdfNode::fromUriString("predicate1").get(),
            LibrdfNode::fromUriString("resource1").get()
    );
    model1.addStatement(statement1);

    LibrdfStatement statement2 = LibrdfStatement::fromRawNodePtrs(
            LibrdfNode::fromUriString("subject2").get(),
            LibrdfNode::fromUriString("predicate2").get(),
            LibrdfNode::fromUriString("resource2").get()
    );
    model1.addStatement(statement2);
//    librdf_model_remove_statement(model1.get(), statement2.get());
    model1.removeStatement(statement2);
    int expected = 1;
    int actual = model1.size();
    ASSERT_EQ(expected, actual);
    model1.freeModel();
    statement1.freeStatement();
    statement2.freeStatement();
}

TEST_F(LibrdfModelTests, TestContext) {
    LibrdfModel model(LibrdfStorage("file").get());
    ASSERT_FALSE(model.supportsContexts());
}




