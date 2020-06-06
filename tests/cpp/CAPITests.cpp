//
// Created by Ciaran on 5/12/2020.
//

#include "gtest/gtest.h"


#include "gtest/gtest.h"
#include "AnnotationSamples.h"
#include "semsim/SemsimCApi.h"
#include "semsim/RDF.h"
#include "SBMLFactory.h"
#include "semsim/SemsimXmlAssistant.h"

using namespace semsim;

class CAPITests : public ::testing::Test {
public:

    AnnotationSamples samples = AnnotationSamples();

    CAPITests() = default;

    void TearDown() override {
    };

};


TEST_F(CAPITests, RDFNew) {
    RDF *rdf_ptr = RDF_new();
    // we access the default base uri to check we have a RDF object
    std::string uri = rdf_ptr->base_uri_;
    std::string expected = "file://./Annotations.rdf";
    ASSERT_STREQ(expected.c_str(), uri.c_str());
    delete rdf_ptr;

}

TEST_F(CAPITests, RDFSize) {
    RDF *rdf_ptr = RDF_new();
    RDF_fromString(rdf_ptr, samples.singular_annotation1.c_str(), "rdfxml", "LannotationsBase.rdf");
    int actual = RDF_size(rdf_ptr);
    int expected = 1;
    ASSERT_EQ(expected, actual);
    delete rdf_ptr;

}

TEST_F(CAPITests, FreeCharStar) {
// validated with valgrind. Not sure how to built into gtest
    char *cstring = _func_that_returns_dynamic_alloc_str();
    free_c_char_star(cstring);
}

TEST_F(CAPITests, RDFToString) {
    RDF *rdf_ptr = RDF_new();
    RDF_fromString(rdf_ptr, samples.singular_annotation1.c_str(), "rdfxml");
    std::string actual = rdf_ptr->toString("rdfxml-abbrev", "annotation.rdf");

    std::cout << actual << std::endl;
    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xml:base=\"file://annotation.rdf\">\n"
                           "  <rdf:Description rdf:about=\"file://./metaid_1\">\n"
                           "    <bqbiol:is rdf:resource=\"https://identifiers.org/uniprot/P0DP23\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n";
    ASSERT_STREQ(expected.c_str(), actual.c_str());
    delete rdf_ptr;
}

TEST_F(CAPITests, RDFgetBaseUri) {
    RDF *rdf_ptr = RDF_new();
    RDF_fromString(rdf_ptr, samples.singular_annotation1.c_str(), "rdfxml");
    char *actual = RDF_getBaseUri(rdf_ptr);
    std::cout << actual << std::endl;
    const char *expected = "file://./Annotations.rdf";
    ASSERT_STREQ(expected, actual);

    free(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, RDFsetBaseUri) {
    RDF *rdf_ptr = RDF_new();
    RDF_fromString(rdf_ptr, samples.singular_annotation1.c_str(), "rdfxml");
    RDF_setBaseUri(rdf_ptr, "ANewBaseUri.rdf");
    char *actual = RDF_getBaseUri(rdf_ptr);
    std::cout << actual << std::endl;
    const char *expected = "file://ANewBaseUri.rdf";
    ASSERT_STREQ(expected, actual);
    free(actual); // necessary because we allocated on heap.
    delete rdf_ptr;

}


TEST_F(CAPITests, RDFqueryResultsAsStr) {
    RDF *rdf_ptr = RDF_new();
    RDF_fromString(rdf_ptr, samples.composite_annotation_pe.c_str(), "rdfxml");

    const char *query = "SELECT ?x ?y ?z \n"
                        "WHERE { ?x ?y ?z }";
    char *results = RDF_queryResultsAsStr(rdf_ptr, query, "csv");
    std::string expected = "x,y,z\r\n"
                           "file://./VLV,http://biomodels.net/biology-qualifiers/isVersionOf,http://identifiers.org/opb/OPB_00154\r\n"
                           "file://./VLV,http://biomodels.net/biology-qualifiers/isPropertyOf,file://./MyModel.rdf#entity_0\r\n"
                           "file://./MyModel.rdf#entity_0,http://biomodels.net/biology-qualifiers/is,http://identifiers.org/fma/FMA:9670\r\n"
                           "file://./MyModel.rdf#entity_0,http://biomodels.net/biology-qualifiers/isPartOf,http://identifiers.org/fma/FMA:18228\r\n";
    ASSERT_STREQ(expected.c_str(), results);
    delete rdf_ptr;
    free_c_char_star(results);
}

TEST_F(CAPITests, TestSingularAnnotationSetAbout) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
    SingularAnnotation_setAbout(singularAnnotation,
                                "metaid6");
    char *actual = SingularAnnotation_getAbout(singularAnnotation);
    const char *expected = "metaid6";
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    SingularAnnotation_delete(singularAnnotation);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestSingularAnnotationSetPredicate) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
    SingularAnnotation_setPredicate(singularAnnotation,
                                    "bqb", "is");
    char *actual = SingularAnnotation_getPredicate(singularAnnotation);
    const char *expected = "http://biomodels.net/biology-qualifiers/is";
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    SingularAnnotation_delete(singularAnnotation);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestEditorPtrMem) {
// verified with valgrind, not sure how to gtest
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    delete rdf_ptr;
    delete editor_ptr;
}

//TEST_F(CAPITests, TestSingularAnnotationSetPredicateNew) {
//    RDF *rdf_ptr = RDF_new();
//
//    Editor *editor_ptr = rdf_ptr->toEditorPtr(
//            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
//            SEMSIM_TYPE_SBML
//    );
//
//    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
//    SingularAnnotation_setPredicateNew(singularAnnotation,
//                                       "UnheardOfNamespace", "NewTerm", "uhon");
//    char *actual = SingularAnnotation_getPredicate(singularAnnotation);
//    const char *expected = "UnheardOfNamespace/NewTerm";
//    ASSERT_STREQ(expected, actual);
//
//    
//    Editor_delete(editor_ptr);
//    SingularAnnotation_delete(singularAnnotation);
//    free_c_char_star(actual);
//}

TEST_F(CAPITests, TestSingularAnnotationSetResourceLiteral) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
    SingularAnnotation_setResourceLiteral(singularAnnotation,
                                          "LiterallyAString");
    char *actual = SingularAnnotation_getResource(singularAnnotation);
    const char *expected = "LiterallyAString";
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    SingularAnnotation_delete(singularAnnotation);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestSingularAnnotationSetResourceUri) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
    SingularAnnotation_setResourceUri(singularAnnotation,
                                      "uniprot:PD98723");
    char *actual = SingularAnnotation_getResource(singularAnnotation);
    const char *expected = "https://identifiers.org/uniprot/PD98723";
    ASSERT_STREQ(expected, actual);

    Editor_delete(editor_ptr);
    SingularAnnotation_delete(singularAnnotation);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestSingularAnnotationSetResourceBlank) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    SingularAnnotation *singularAnnotation = SingularAnnotation_new(editor_ptr);
    SingularAnnotation_setResourceBlank(singularAnnotation, "Nothing");
    char *actual = SingularAnnotation_getResource(singularAnnotation);
    const char *expected = "Nothing";
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    SingularAnnotation_delete(singularAnnotation);
    free_c_char_star(actual);
    delete rdf_ptr;
}


TEST_F(CAPITests, TestPhysicalEntity) {
    RDF *rdf_ptr = RDF_new();

    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );

    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setAbout(physical_entity_ptr, "metaid87");
    physical_entity_ptr = PhysicalEntity_setPhysicalProperty(physical_entity_ptr, "opb/OPB007");
    physical_entity_ptr = PhysicalEntity_setIdentity(physical_entity_ptr, "uniprot:PD58736");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "fma:FMA_8764");

    char *actual = PhysicalEntity_str(physical_entity_ptr, "rdfxml-abbrev", "./annotations.rdf");
    const char *expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xml:base=\"file://./annotations.rdf\">\n"
                           "  <rdf:Description rdf:about=\"PhysicalEntity0000\">\n"
                           "    <bqbiol:is rdf:resource=\"https://identifiers.org/uniprot/PD58736\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/fma/FMA_8764\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"metaid87\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalEntity0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/OPB007\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n";
    std::cout << actual <<
              std::endl;
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    PhysicalEntity_delete(physical_entity_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestPhysicalEntityAbout) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setAbout(physical_entity_ptr, "metaid87");
    char *actual = PhysicalEntity_getAbout(physical_entity_ptr);
    const char *expected = "metaid87";
    std::cout << actual << std::endl;
    ASSERT_STREQ(expected, actual);

    Editor_delete(editor_ptr);
    free_c_char_star(actual);
    // we need to free the node inside get about manually here because
    // we are not using the str() method
    PhysicalEntity_freeAll(physical_entity_ptr);
    delete rdf_ptr;
}


TEST_F(CAPITests, TestPhysicalEntityPhysicalProperty) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setPhysicalProperty(physical_entity_ptr, "opb/opb_465");
    char *actual = PhysicalEntity_getPhysicalProperty(physical_entity_ptr);
    const char *expected = "https://identifiers.org/opb/opb_465";
    std::cout << actual <<
              std::endl;
    ASSERT_STREQ(expected, actual);

    Editor_delete(editor_ptr);
    // we must free this node manually here. Cannot be incorporated into
    // delete function because it'll clash with the main use case (i.e. str method)
    PhysicalEntity_freeAll(physical_entity_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestPhysicalEntityGetIdentity) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setIdentity(physical_entity_ptr, "uniprot/PD7363");
    char *actual = PhysicalEntity_getIdentity(physical_entity_ptr);
    const char *expected = "https://identifiers.org/uniprot/PD7363";
    std::cout << actual <<
              std::endl;
    ASSERT_STREQ(expected, actual);


    Editor_delete(editor_ptr);
    // as opposed to PhysicalEntity_delete which would leave behind un-freed nodes.
    PhysicalEntity_freeAll(physical_entity_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestPhysicalEntityLocations) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8376");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8377");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8378");
    int num_locations = PhysicalEntity_getNumLocations(physical_entity_ptr);
    const char *expected = "https://identifiers.org/FMA/fma:8376\n"
                           "https://identifiers.org/FMA/fma:8377\n"
                           "https://identifiers.org/FMA/fma:8378\n";
    std::ostringstream os;
    for (int i = 0; i < num_locations; i++) {
        char *string = PhysicalEntity_getLocation(physical_entity_ptr, i);
        os << string << '\n';
        free_c_char_star(string);
    }
    ASSERT_STREQ(expected, os.str().c_str());
    PhysicalEntity_freeAll(physical_entity_ptr);
    Editor_delete(editor_ptr);
    delete rdf_ptr;

}


TEST_F(CAPITests, TestPhysicalEntityNumLocations) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8376");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8377");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8378");
    int expected = 3;
    int actual = PhysicalEntity_getNumLocations(physical_entity_ptr);
    ASSERT_EQ(expected, actual);

    Editor_delete(editor_ptr);
    PhysicalEntity_freeAll(physical_entity_ptr);
    delete rdf_ptr;
}


TEST_F(CAPITests, TestPhysicalEntityStr) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setPhysicalProperty(physical_entity_ptr, "opb/opb_465");
    physical_entity_ptr = PhysicalEntity_setAbout(physical_entity_ptr, "metaid87");
    physical_entity_ptr = PhysicalEntity_setIdentity(physical_entity_ptr, "uniprot/PD7363");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8376");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8377");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8378");
    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xml:base=\"file://./Annot.rdf\">\n"
                           "  <rdf:Description rdf:about=\"PhysicalEntity0000\">\n"
                           "    <bqbiol:is rdf:resource=\"https://identifiers.org/uniprot/PD7363\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8376\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8377\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8378\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"metaid87\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalEntity0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb_465\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n";

    char *actual = PhysicalEntity_str(physical_entity_ptr, "rdfxml-abbrev", "./Annot.rdf");
    std::cout << actual << std::endl;
    ASSERT_STREQ(expected.c_str(), actual);

    Editor_delete(editor_ptr);
    PhysicalEntity_delete(physical_entity_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestPhysicalProcess) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalProcess *physical_process_ptr = PhysicalProcess_new(editor_ptr);

    physical_process_ptr = PhysicalProcess_setAbout(physical_process_ptr, "Metaid0937");
    physical_process_ptr = PhysicalProcess_setPhysicalProperty(physical_process_ptr, "opb/opb93864");
    physical_process_ptr = PhysicalProcess_addSink(
            physical_process_ptr, "Sink9", 1.0, "Entity8");
    physical_process_ptr = PhysicalProcess_addSource(
            physical_process_ptr, "Source1", 1.0, "Entity8");
    physical_process_ptr = PhysicalProcess_addMediator(
            physical_process_ptr, "Mod4", 1.0, "Entity8");

    char *actual = PhysicalProcess_str(physical_process_ptr, "rdfxml-abbrev", "./annotations.rdf");
    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xmlns:semsim=\"http://www.bhi.washington.edu/semsim#\"\n"
                           "   xml:base=\"file://./annotations.rdf\">\n"
                           "  <rdf:Description rdf:about=\"Metaid0937\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalProcess0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb93864\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Mod4\">\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"PhysicalProcess0000\">\n"
                           "    <semsim:hasMediatorParticipant rdf:resource=\"Mod4\"/>\n"
                           "    <semsim:hasSinkParticipant rdf:resource=\"Sink9\"/>\n"
                           "    <semsim:hasSourceParticipant rdf:resource=\"Source1\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Sink9\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Source1\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n";
    std::cout << actual << std::endl;
    ASSERT_STREQ(expected.c_str(), actual);

    Editor_delete(editor_ptr);
    PhysicalProcess_delete(physical_process_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;
}

TEST_F(CAPITests, TestPhysicalForce) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalForce *physical_force_ptr = PhysicalForce_new(editor_ptr);

    physical_force_ptr = PhysicalForce_setAbout(physical_force_ptr, "Metaid0937");
    physical_force_ptr = PhysicalForce_setPhysicalProperty(physical_force_ptr, "opb/opb93864");
    physical_force_ptr = PhysicalForce_addSink(
            physical_force_ptr, "Sink9", 1.0, "Entity8");
    physical_force_ptr = PhysicalForce_addSource(
            physical_force_ptr, "Source1", 1.0, "Entity9");

    char *actual = PhysicalForce_str(physical_force_ptr, "rdfxml-abbrev", "./Annot.rdf");
    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xmlns:semsim=\"http://www.bhi.washington.edu/semsim#\"\n"
                           "   xml:base=\"file://./Annot.rdf\">\n"
                           "  <rdf:Description rdf:about=\"Metaid0937\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalForce0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb93864\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"PhysicalForce0000\">\n"
                           "    <semsim:hasSinkParticipant rdf:resource=\"Sink9\"/>\n"
                           "    <semsim:hasSourceParticipant rdf:resource=\"Source1\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Sink9\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Source1\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity9\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n"
                           "";
    std::cout << actual << std::endl;
    ASSERT_STREQ(expected.c_str(), actual);


    delete rdf_ptr;
    Editor_delete(editor_ptr);
    PhysicalForce_delete(physical_force_ptr);
    free_c_char_star(actual);
}

TEST_F(CAPITests, TestEditorToRDF) {
    RDF *rdf_ptr = RDF_new();
    Editor *editor_ptr = rdf_ptr->toEditorPtr(
            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
            SEMSIM_TYPE_SBML
    );
    PhysicalProcess *physical_process_ptr = PhysicalProcess_new(editor_ptr);

    physical_process_ptr = PhysicalProcess_setAbout(physical_process_ptr, "Metaid0936");
    physical_process_ptr = PhysicalProcess_setPhysicalProperty(physical_process_ptr, "opb/opb93864");
    physical_process_ptr = PhysicalProcess_addSink(
            physical_process_ptr, "Sink9", 1.0, "Entity8");
    physical_process_ptr = PhysicalProcess_addSource(
            physical_process_ptr, "Source1", 1.0, "Entity8");
    physical_process_ptr = PhysicalProcess_addMediator(
            physical_process_ptr, "Mod4", 1.0, "Entity8");

    PhysicalEntity *physical_entity_ptr = PhysicalEntity_new(editor_ptr);
    physical_entity_ptr = PhysicalEntity_setPhysicalProperty(physical_entity_ptr, "opb/opb_465");
    physical_entity_ptr = PhysicalEntity_setAbout(physical_entity_ptr, "metaid87");
    physical_entity_ptr = PhysicalEntity_setIdentity(physical_entity_ptr, "uniprot/PD7363");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8376");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8377");
    physical_entity_ptr = PhysicalEntity_addLocation(physical_entity_ptr, "FMA:fma:8378");

    PhysicalForce *physical_force_ptr = PhysicalForce_new(editor_ptr);

    physical_force_ptr = PhysicalForce_setAbout(physical_force_ptr, "Metaid0937");
    physical_force_ptr = PhysicalForce_setPhysicalProperty(physical_force_ptr, "opb/opb93864");
    physical_force_ptr = PhysicalForce_addSink(
            physical_force_ptr, "Sink9", 1.0, "Entity8");
    physical_force_ptr = PhysicalForce_addSource(
            physical_force_ptr, "Source1", 1.0, "Entity9");


    Editor_addPhysicalProcess(editor_ptr, physical_process_ptr
    );
    Editor_addPhysicalEntity(editor_ptr, physical_entity_ptr
    );
    Editor_addPhysicalForce(editor_ptr, physical_force_ptr
    );
    Editor_toRDF(editor_ptr);

    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                           "   xmlns:semsim=\"http://www.bhi.washington.edu/semsim#\"\n"
                           "   xml:base=\"file://./Annot.rdf\">\n"
                           "  <rdf:Description rdf:about=\"Metaid0936\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalProcess0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb93864\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Metaid0937\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalForce0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb93864\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Mod4\">\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"PhysicalEntity0000\">\n"
                           "    <bqbiol:is rdf:resource=\"https://identifiers.org/uniprot/PD7363\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8376\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8377\"/>\n"
                           "    <bqbiol:isPartOf rdf:resource=\"https://identifiers.org/FMA/fma:8378\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"PhysicalForce0000\">\n"
                           "    <semsim:hasSinkParticipant rdf:resource=\"Sink9\"/>\n"
                           "    <semsim:hasSourceParticipant rdf:resource=\"Source1\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"PhysicalProcess0000\">\n"
                           "    <semsim:hasMediatorParticipant rdf:resource=\"Mod4\"/>\n"
                           "    <semsim:hasSinkParticipant rdf:resource=\"Sink9\"/>\n"
                           "    <semsim:hasSourceParticipant rdf:resource=\"Source1\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Sink9\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"Source1\">\n"
                           "    <semsim:hasMultiplier rdf:datatype=\"http://www.w3.org/2001/XMLSchema#double\">1</semsim:hasMultiplier>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity8\"/>\n"
                           "    <semsim:hasPhysicalEntityReference rdf:resource=\"Entity9\"/>\n"
                           "  </rdf:Description>\n"
                           "  <rdf:Description rdf:about=\"metaid87\">\n"
                           "    <bqbiol:isPropertyOf rdf:resource=\"PhysicalEntity0000\"/>\n"
                           "    <bqbiol:isVersionOf rdf:resource=\"https://identifiers.org/opb/opb_465\"/>\n"
                           "  </rdf:Description>\n"
                           "</rdf:RDF>\n";
    char *actual = RDF_toString(rdf_ptr, "rdfxml-abbrev", "./Annot.rdf");
    std::cout << actual << std::endl;


    Editor_delete(editor_ptr);
    PhysicalEntity_delete(physical_entity_ptr);
    PhysicalProcess_delete(physical_process_ptr);
    PhysicalForce_delete(physical_force_ptr);
    free_c_char_star(actual);
    delete rdf_ptr;

}

/*
 * todo support for equality operators
 */

TEST_F(CAPITests, TestRDFTwice1) {
    RDF *rdf_ptr1 = RDF_new();
    RDF *rdf_ptr2 = RDF_new();
    ASSERT_NE(rdf_ptr1, rdf_ptr2);
    delete rdf_ptr1;
    delete rdf_ptr2;
}

TEST_F(CAPITests, TestRDFTwice2) {
    RDF *rdf_ptr1 = RDF_new();
    RDF *rdf_ptr2 = RDF_new();
    ASSERT_NE(rdf_ptr1->getModel(), rdf_ptr2->getModel());
    delete rdf_ptr1;
    delete rdf_ptr2;
}

TEST_F(CAPITests, TestRDFTwice3) {
    RDF *rdf_ptr1 = RDF_new();
    RDF *rdf_ptr2 = RDF_new();
    ASSERT_NE(rdf_ptr1->getStorage(), rdf_ptr2->getStorage());
    delete rdf_ptr1;
    delete rdf_ptr2;


}

//
//TEST_F(CAPITests, TestAddTwoAnn) {
//    RDF *rdf_ptr1 = RDF_new();
//    Editor *editor_ptr1 = rdf_ptr1->toEditorPtr(
//            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
//            SEMSIM_TYPE_SBML
//    );
//    // first annotation
//    SingularAnnotation *annotation1 = SingularAnnotation_new(editor_ptr1);
//    annotation1 = SingularAnnotation_setAbout(annotation1, "first about");
//    annotation1 = SingularAnnotation_setPredicate(annotation1, "bqb", "is");
//    annotation1 = SingularAnnotation_setResourceLiteral(annotation1, "First Literal Resource");
//    Editor_addSingleAnnotation(editor_ptr1, annotation1);
//
//    // second annotation
//    SingularAnnotation *annotation2 = SingularAnnotation_new(editor_ptr1);
//    annotation2 = SingularAnnotation_setAbout(annotation2, "Second about");
//    annotation2 = SingularAnnotation_setPredicate(annotation2, "bqb", "is");
//    annotation2 = SingularAnnotation_setResourceLiteral(annotation2, "second Literal Resource");
//    Editor_addSingleAnnotation(editor_ptr1, annotation2);
//    Editor_toRDF(editor_ptr1);
//    char *string = RDF_toString(rdf_ptr1, "rdfxml-abbrev", "./Annot.rdf");
//    std::string observed_string = (const char *) string;
//
//
//    std::cout << observed_string << "\n\n" << observed_string << std::endl;
//    std::string expected = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
//                           "<rdf:RDF xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\"\n"
//                           "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
//                           "   xml:base=\"file://./Annot.rdf\">\n"
//                           "  <rdf:Description rdf:about=\"Second about\">\n"
//                           "    <bqbiol:is rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#string\">second Literal Resource</bqbiol:is>\n"
//                           "  </rdf:Description>\n"
//                           "  <rdf:Description rdf:about=\"first about\">\n"
//                           "    <bqbiol:is rdf:datatype=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#string\">First Literal Resource</bqbiol:is>\n"
//                           "  </rdf:Description>\n"
//                           "</rdf:RDF>\n";
//    ASSERT_STREQ(expected.c_str(), observed_string.c_str());
//
//
//    Editor_delete(editor_ptr1);
//    free(string);
//    SingularAnnotation_delete(annotation1);
//    SingularAnnotation_delete(annotation2);
//}

//TEST_F(CAPITests, TestRDFTwic) {
//    RDF *rdf_ptr1 = RDF_new();
//    Editor *editor_ptr1 = rdf_ptr1->toEditorPtr(
//            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
//            SEMSIM_TYPE_SBML
//    );
//    SingularAnnotation *annotation1 = SingularAnnotation_new(editor_ptr1);
//    annotation1 = SingularAnnotation_setAbout(annotation1, "first about");
//    annotation1 = SingularAnnotation_setPredicate(annotation1, "bqb", "is");
//    annotation1 = SingularAnnotation_setResourceLiteral(annotation1, "First Literal Resource");
//    Editor_addSingleAnnotation(editor_ptr1, annotation1);
//    Editor_toRDF(editor_ptr1);
//    char *string1 = RDF_toString(rdf_ptr1, "rdfxml-abbrev", "./Annot.rdf");
//    std::string observed_string1 = (const char *) string1;
//
//
////    free_world(World::getWorld());
//
//
//    RDF *rdf_ptr2 = RDF_new();
//    Editor *editor_ptr2 = rdf_ptr2->toEditorPtr(
//            SBMLFactory::getSBMLString(SBML_NOT_ANNOTATED),
//            SEMSIM_TYPE_SBML
//    );
//    SingularAnnotation *annotation2 = SingularAnnotation_new(editor_ptr2);
//    annotation2 = SingularAnnotation_setAbout(annotation2, "Second about");
//    annotation2 = SingularAnnotation_setPredicate(annotation2, "bqb", "is");
//    annotation2 = SingularAnnotation_setResourceLiteral(annotation2, "second Literal Resource");
//    Editor_addSingleAnnotation(editor_ptr2, annotation2);
//    Editor_toRDF(editor_ptr2);
//    char *string2 = RDF_toString(rdf_ptr2, "rdfxml-abbrev", "./Annot.rdf");
//    std::string observed_string2 = (const char *) string2;
//
//
////    std::cout << observed_string1 << "\n\n" << observed_string2 << std::endl;
//
////    ASSERT_STRNE(observed_string1.c_str(), observed_string2.c_str());
//    std::cout << "rdf_ptr, " << rdf_ptr1 << ", " << rdf_ptr2 << std::endl;
//    std::cout << "rdf_ptr.getModel, " << rdf_ptr1->getModel() << ", " << rdf_ptr2->getModel() << std::endl;
//    std::cout << "editor_ptr, " << editor_ptr1 << ", " << editor_ptr2 << std::endl;
//    std::cout << "annotation, " << annotation1 << ", " << annotation2 << std::endl;
//    std::cout << "string, " << string1 << ", " << string2 << std::endl;
//
//
//    Editor_delete(editor_ptr1);
//    free_c_char_star(string1);
//    SingularAnnotation_delete(annotation1);
//    
//    SingularAnnotation_delete(annotation1);
//
//
//    
//    Editor_delete(editor_ptr2);
//    free_c_char_star(string2);
//    SingularAnnotation_delete(annotation2);
//
//}















































