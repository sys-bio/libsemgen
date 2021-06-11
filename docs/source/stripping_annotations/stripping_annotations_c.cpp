#include "omexmeta/OmexMetaCApi.h"
#include <iostream>

using namespace omexmeta;

int main(){
    const char* sbml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                       "<sbml xmlns=\"http://www.sbml.org/sbml/level3/version1/core\" level=\"3\" version=\"1\">\n"
                       "    <model metaid=\"ToyModel\" id=\"ToyModel\">\n"
                       "        <listOfCompartments>\n"
                       "            <compartment id=\"cytosol\" metaid=\"comp1\" spatialDimensions=\"3\" size=\"1\" constant=\"true\"/>\n"
                       "            <compartment id=\"extraCell\" metaid=\"comp2\" spatialDimensions=\"3\" size=\"10\" constant=\"true\"/>\n"
                       "        </listOfCompartments>\n"
                       "        <listOfSpecies>\n"
                       "            <species id=\"A\" metaid=\"sp_1\" compartment=\"cytosol\" initialConcentration=\"10\" hasOnlySubstanceUnits=\"false\"\n"
                       "                     boundaryCondition=\"false\" constant=\"false\">\n"
                       "                <annotation>\n"
                       "                    <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
                       "                             xmlns:bqbiol=\"http://biomodels.net/biology-qualifiers/\">\n"
                       "                        <rdf:Description rdf:about=\"#DescriptionAnnotationWithBag\">\n"
                       "                            <bqbiol:is>\n"
                       "                                <rdf:Bag>\n"
                       "                                    <rdf:li rdf:resource=\"http://identifiers.org/uniprot/P00742\"/>\n"
                       "                                    <rdf:li rdf:resource=\"http://identifiers.org/uniprot/P00743\"/>\n"
                       "                                </rdf:Bag>\n"
                       "                            </bqbiol:is>\n"
                       "                        </rdf:Description>\n"
                       "                    </rdf:RDF>\n"
                       "                </annotation>\n"
                       "            </species>\n"
                       "            <species id=\"B\" metaid=\"sp_2\" compartment=\"cytosol\" initialConcentration=\"0\" hasOnlySubstanceUnits=\"false\"\n"
                       "                     boundaryCondition=\"false\" constant=\"false\"/>\n"
                       "            <species id=\"Ca\" metaid=\"sp_3\" compartment=\"cytosol\" initialConcentration=\"2\" hasOnlySubstanceUnits=\"false\"\n"
                       "                     boundaryCondition=\"false\" constant=\"false\"/>\n"
                       "            <species id=\"PlasmaCa\" metaid=\"sp_4\" compartment=\"extraCell\" initialConcentration=\"3\"\n"
                       "                     hasOnlySubstanceUnits=\"false\" boundaryCondition=\"false\" constant=\"false\"/>\n"
                       "            <species id=\"Enzyme\" metaid=\"sp_5\" compartment=\"cytosol\" initialConcentration=\"2\"\n"
                       "                     hasOnlySubstanceUnits=\"false\" boundaryCondition=\"false\" constant=\"false\"/>\n"
                       "        </listOfSpecies>\n"
                       "        <listOfParameters>\n"
                       "            <parameter id=\"k1\" value=\"0.1\" constant=\"true\"/>\n"
                       "            <parameter id=\"k2\" value=\"0.1\" constant=\"true\"/>\n"
                       "        </listOfParameters>\n"
                       "        <listOfReactions>\n"
                       "            <reaction id=\"r1\" metaid=\"react1\" reversible=\"false\" fast=\"false\">\n"
                       "                <listOfReactants>\n"
                       "                    <speciesReference species=\"B\" stoichiometry=\"1\" constant=\"true\"/>\n"
                       "                </listOfReactants>\n"
                       "                <listOfProducts>\n"
                       "                    <speciesReference species=\"A\" stoichiometry=\"2\" constant=\"true\"/>\n"
                       "                </listOfProducts>\n"
                       "                <kineticLaw>\n"
                       "                    <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
                       "                        <apply>\n"
                       "                            <times/>\n"
                       "                            <ci>k2</ci>\n"
                       "                            <ci>B</ci>\n"
                       "                        </apply>\n"
                       "                    </math>\n"
                       "                </kineticLaw>\n"
                       "            </reaction>\n"
                       "            <reaction id=\"r2\" metaid=\"react2\" reversible=\"false\" fast=\"false\">\n"
                       "                <listOfReactants>\n"
                       "                    <speciesReference species=\"Ca\" stoichiometry=\"1\" constant=\"true\"/>\n"
                       "                    <speciesReference species=\"A\" stoichiometry=\"1\" constant=\"true\"/>\n"
                       "                </listOfReactants>\n"
                       "                <listOfProducts>\n"
                       "                    <speciesReference species=\"PlasmaCa\" stoichiometry=\"1\" constant=\"true\"/>\n"
                       "                </listOfProducts>\n"
                       "                <listOfModifiers>\n"
                       "                    <modifierSpeciesReference species=\"Enzyme\"/>\n"
                       "                </listOfModifiers>\n"
                       "                <kineticLaw>\n"
                       "                    <math xmlns=\"http://www.w3.org/1998/Math/MathML\">\n"
                       "                        <apply>\n"
                       "                            <times/>\n"
                       "                            <ci>k2</ci>\n"
                       "                            <ci>Ca</ci>\n"
                       "                        </apply>\n"
                       "                    </math>\n"
                       "                </kineticLaw>\n"
                       "            </reaction>\n"
                       "        </listOfReactions>\n"
                       "    </model>\n"
                       "</sbml>";

    RDF* rdf_ptr = RDF_new();
    Editor* editor_ptr = RDF_toEditor(rdf_ptr, sbml, false, false);
    char* stripped = Editor_stripAnnotations(editor_ptr);
    printf("%s\n", stripped);
    free(stripped);
}