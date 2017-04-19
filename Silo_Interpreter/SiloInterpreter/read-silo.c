#include <stdlib.h>
#include <string.h>
#include <silo.h>
#include <math.h>
#include <hdf5.h>

#define COEFF_TYPE double

//usage ./read-silo <infile> <outfile>

static void read_material_object(DBmaterial *material, DBcsgmesh *csgmesh, FILE *outfile);
static void read_csg_object(DBcsgmesh *csgmesh, FILE *outfile);
static void parse_mesh_typeflags(DBcsgmesh *csgmesh, int i, int *coeffIndex, FILE *outfile);
void parse_mesh_zonelist(DBcsgzonelist *zones, FILE *outfile);

int main(int argc, char **argv){
    DBfile        *dbfile;
    DBtoc  *dbtoc;
	DBcsgmesh *csgmesh;
	DBmaterial *material;
	int            i=1, driver = DB_UNKNOWN;
	FILE* outfile = fopen(argv[2], "w");
    DBShowErrors(DB_ALL, NULL);
	
    dbfile = DBOpen(argv[1], driver, DB_READ);
    dbtoc  = DBGetToc(dbfile);
	DBSetDir(dbfile, "domain0");
	dbtoc = DBGetToc(dbfile);
	
	
	csgmesh = DBGetCsgmesh(dbfile, dbtoc->csgmesh_names[0]);
	read_csg_object(csgmesh, outfile);
	

	material = DBGetMaterial(dbfile, dbtoc->mat_names[0]);
	read_material_object(material, csgmesh, outfile);
	
	DBClose(dbfile);


    return 0;
}

static void
read_material_object(DBmaterial *material, DBcsgmesh *csgmesh, FILE *outfile)
{
	//print matcount
	printf("Material Count: %i\n", material->nmat);
	fprintf(outfile, "%i\n", material->nmat);
	
	for(int i = 0; i < material->nmat; i++){
		printf("Material Index: %i\n", material->matnos[i]);
		fprintf(outfile, "%i\n", material->matnos[i]);
		
		printf("\tMaterial Name: %s\n", material->matnames[i]);
		fprintf(outfile, "%s\n", material->matnames[i]);

		//if hex color code
		if(material->matcolors[i][0] == '#'){
			printf("\tMaterial Color: %s\n", material->matcolors[i]);
			fprintf(outfile, "%s\n", material->matcolors[i]);
		} else {
			printf("\tMaterial Color: %s\n", "#808080");
			fprintf(outfile, "%s\n", "#808080");

		}
	}

	for(int i = 0; i < csgmesh->zones->nzones; i++){
		printf("Zone Mat: %i\n", material->matlist[i]);
		fprintf(outfile, "%i\n", material->matlist[i]);

	}

}

static void
read_csg_object(DBcsgmesh *csgmesh, FILE *outfile)
{

	//print name
	printf("Name: %s\n", csgmesh->name);
	fprintf(outfile, "%s\n", csgmesh->name);
	//block no && group no
	printf("Block Number: %i\nGroup Number: %i\n", csgmesh->block_no, csgmesh->group_no);
	fprintf(outfile, "%i\n%i\n", csgmesh->block_no, csgmesh->group_no);
	//cycle number
	printf("Cycle: %i\n", csgmesh->cycle);
	fprintf(outfile, "%i\n", csgmesh->cycle);
	//print units
	printf("Units: %s, %s, %s\n", csgmesh->units[0], csgmesh->units[1], csgmesh->units[2]);
	fprintf(outfile, "%s %s %s\n", csgmesh->units[0], csgmesh->units[1], csgmesh->units[2]);
	//print labels
	printf("Labels: %s, %s, %s\n", csgmesh->labels[0], csgmesh->labels[1], csgmesh->labels[2]);
	fprintf(outfile, "%s %s %s\n", csgmesh->labels[0], csgmesh->labels[1], csgmesh->labels[2]);
	//print dimensions
	printf("Dimensions: %i\n", csgmesh->ndims);
	fprintf(outfile, "%i\n", csgmesh->ndims);
	//coeff array size
	printf("Coefficients: %i\n", csgmesh->lcoeffs);
	fprintf(outfile, "%i\n", csgmesh->lcoeffs);
	

	//print type flags
		int coeffIndex = 0;
	printf("Number of Types: %i\n", csgmesh->nbounds);
	fprintf(outfile, "%i\n", csgmesh->nbounds);
	
	for(int i = 0 ; i < csgmesh->nbounds; i++){
		parse_mesh_typeflags(csgmesh, i, &coeffIndex, outfile);	
	}

	//parse zonelist
	parse_mesh_zonelist(csgmesh->zones, outfile);	
}

void parse_mesh_zonelist(DBcsgzonelist *zones, FILE *outfile){
	printf("Zonelist:\n");
	int nregs;
	int nzones;
	char type[256];
	//print num of regions
	printf("Number of Regions: %i\n", nregs = zones->nregs);
	fprintf(outfile, "%i\n", nregs = zones->nregs);
	//print origin
	printf("Origin: %i\n", zones->origin);
	fprintf(outfile, "%i\n", zones->origin);
	
	//print flags and ids
	for(int i = 0; i < nregs; i++){
		//TODO verbal parsing
		switch(zones->typeflags[i]){
			case DBCSG_INNER:
				strcpy(type,"Inner");
				break;
			case DBCSG_OUTER:
				strcpy(type, "Outer");
				break;
			case DBCSG_ON:
				strcpy(type, "On");
				break;
			case DBCSG_UNION:
				strcpy(type, "Union");
				break;
			case DBCSG_INTERSECT:
				strcpy(type, "Intersect");
				break;
			case DBCSG_DIFF:
				strcpy(type, "Diff");
				break;
			case DBCSG_COMPLIMENT:
				strcpy(type, "Compliment");
				break;
			case DBCSG_XFORM:
				strcpy(type, "Xform");
				break;
			case DBCSG_SWEEP:
				strcpy(type, "Sweep");
				break;
		}
		printf("Region %i:\n\tTypeflag: %s -- Leftid: %i -- rightid: %i\n",
				i, type, zones->leftids[i], zones->rightids[i]);
		fprintf(outfile, "%s\n%i\n%i\n",
				type, zones->leftids[i], zones->rightids[i]);

	}
	
	//print number of zones
	printf("Number of zones: %i\n", nzones=zones->nzones);
	fprintf(outfile, "%i\n", nzones=zones->nzones);

	//print zones
	for(int i = 0; i < nzones; i++){
		printf("Zone %i: %i\n", i, zones->zonelist[i]);
		fprintf(outfile, "%i\n", zones->zonelist[i]);
	}

}

static void 
parse_mesh_typeflags(DBcsgmesh *csgmesh, int i, int *coeffIndex, FILE *outfile){
	switch (csgmesh->typeflags[i]){
		case DBCSG_QUADRIC_G:
			printf("Typeflag%i: %s\n", i, "Quadric");
			fprintf(outfile, "%s\n", "Quadric");
			for(int j = 0; j < 10; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_SPHERE_PR:
			printf("Typeflag%i: %s\n", i, "Sphere");
			fprintf(outfile, "%s\n", "Sphere");
			for(int j = 0; j < 4; j++, ((*coeffIndex))++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_ELLIPSOID_PRRR:
			printf("Typeflag%i: %s\n", i, "Ellipsoid");
			fprintf(outfile, "%s\n", "Ellipsoid");
			for(int j = 0; j < 6; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_G:
			printf("Typeflag%i: %s\n", i, "Plane_g");
			fprintf(outfile, "%s\n", "Plane_g");
			for(int j = 0; j < 4; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_X:
			printf("Typeflag%i: %s\n", i, "Plane_x");
			fprintf(outfile, "%s\n", "Plane_x");
			for(int j = 0; j < 1; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_Y:
			printf("Typeflag%i: %s\n", i, "Plane_y");
			fprintf(outfile, "%s\n", "Plane_y");
			for(int j = 0; j < 1; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_Z:
			printf("Typeflag%i: %s\n", i, "Plane_z");
			fprintf(outfile, "%s\n", "Plane_z");
			for(int j = 0; j < 1; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_PN:
			printf("Typeflag%i: %s\n", i, "Plane_pn");
			fprintf(outfile, "%s\n", "Plane_pn");
			for(int j = 0; j < 6; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PLANE_PPP:
			printf("Typeflag%i: %s\n", i, "Plane_ppp");
			fprintf(outfile, "%s\n", "Plane_ppp");
			for(int j = 0; j < 9; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_CYLINDER_PNLR:
			printf("Typeflag%i: %s\n", i, "Cylinder_pnlr");
			fprintf(outfile, "%s\n", "Cylinder_pnlr");
			for(int j = 0; j < 8; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_CYLINDER_PPR:
			printf("Typeflag%i: %s\n", i, "Cylinder_ppr");
			fprintf(outfile, "%s\n", "Cylinder_ppr");
			for(int j = 0; j < 7; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_BOX_XYZXYZ:
			printf("Typeflag%i: %s\n", i, "Box");
			fprintf(outfile, "%s\n", "Box");
			for(int j = 0; j < 6; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_CONE_PNLA:
			printf("Typeflag%i: %s\n", i, "Cone_pnla");
			fprintf(outfile, "%s\n", "Cone_pnla");
			for(int j = 0; j < 8; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_CONE_PPA:
			printf("Typeflag%i: %s\n", i, "Cone_ppa");
			fprintf(outfile, "%s\n", "Cone_ppa");
			//TODO
			break;
		case DBCSG_POLYHEDRON_KF:
			printf("Typeflag%i: %s\n", i, "Polyhedron");
			fprintf(outfile, "%s\n", "Polyhedron");
			//TODO
			break;
		case DBCSG_HEX_6F:
			printf("Typeflag%i: %s\n", i, "Hex");
			fprintf(outfile, "%s\n", "Hex");
			for(int j = 0; j < 36; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_TET_4F:
			printf("Typeflag%i: %s\n", i, "Tet");
			fprintf(outfile, "%s\n", "Tet");
			for(int j = 0; j < 24; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PYRAMID_5F:
			printf("Typeflag%i: %s\n", i, "Pyramid");
			fprintf(outfile, "%s\n",  "Pyramid");
			for(int j = 0; j < 30; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
		case DBCSG_PRISM_5F:
			printf("Typeflag%i: %s\n", i, "Prism");
			fprintf(outfile, "%s\n", "Prism");
			for(int j = 0; j < 30; j++, (*coeffIndex)++){
				printf("\tCoefficient %i: %f\n", j, ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
				fprintf(outfile, "%f\n", ((COEFF_TYPE*)((*csgmesh).coeffs))[(*coeffIndex)]);
			}
			break;
	}
}
