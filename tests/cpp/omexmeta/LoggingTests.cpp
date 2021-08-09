//
// Created by Ciaran on 05/08/2021.
//

#include "SBMLFactory.h"
#include "omexmeta/OmexMeta.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "gtest/gtest.h"

using namespace omexmeta;


class LoggingTests : public ::testing::Test {
public:
    std::filesystem::path p = std::filesystem::current_path() / "log.log";
    LoggingTests() = default;

    ~LoggingTests() override{
        if (std::filesystem::exists(p)){
            try {
                std::filesystem::remove(p);
            } catch (std::exception& e){
                OMEX_INFO(e.what());
            }
        }
    }
};

TEST_F(LoggingTests, DefaultLevel){
    auto actual = Logger::getLogger()->getLevel();
    auto expected = Logger::LogLevel::warn;
    ASSERT_EQ(expected, actual);
}

TEST_F(LoggingTests, UseConsoleLoggerOutOfTheBox){
    // needs to be called at least once, otherwise we'll
    // not have init the omexmeta logger, and we'll just
    // be using the spdlog defaults.
    Logger::getLogger();
    // should not output to console
    OMEX_INFO("Not displayed to console");
    // should output to console
    OMEX_WARN("Displayed to console");
}

TEST_F(LoggingTests, SwitchToFileLogger){
    std::filesystem::path fname = std::filesystem::current_path() / "log.log";
    Logger::getLogger()->fileLogger(fname);
    // should not output to console
    OMEX_INFO("Not displayed to console");
    // should output to console
    OMEX_WARN("Displayed to console");
    ASSERT_TRUE(std::filesystem::exists(fname));
}

TEST_F(LoggingTests, UseFileLogger) {
    Logger::getLogger()->setLevel(Logger::LogLevel::info);
    Logger::getLogger()->fileLogger(p);
    std::string sbml = SBMLFactory::getSBML(SBML_INVALID_METAIDS);
    RDF rdf = RDF::fromString(sbml, "turtle");
    ASSERT_TRUE(std::filesystem::exists(p));
}






















