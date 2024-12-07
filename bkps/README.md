
# Black Key Provisioning Service (BKPS)

The Black Key Provisioning Service is a suite of applications used to securely provision an
AES root key to Altera Agilex 5 Device. Black key provisioning involves a multi-step process
to establish a mutually authenticated secure connection between an SDM-based device at an
untrusted location and an instance of black key provisioning service that runs on a trusted
system in a trusted location. All connections between the BKP service and tools are
authenticated using mTLS. The mutually authenticated secure connection helps to ensure
that the black key provisioning service can program the AES root key and other confidential
information without exposing any data to an intermediate or third party

## Development

To start your application in the dev profile, simply run:

    ../gradlew

## Building for production

### Packaging as jar

To build the final jar and optimize application for production, run:

    ../gradlew -Pprod clean bootJar

To ensure everything worked, run:

    java -jar build/libs/*.jar

### Packaging as war

To package your application as a war in order to deploy it to an application server, run:

    ../gradlew -Pprod -Pwar clean bootWar

### Build SQL scheme file using Liquibase

To build SQL scheme file using Liquibase for production and AWS, run:

    ../gradlew -Pprod -Paws liquibaseGenerateSql -Pversion=${version} -Dversion=${version}

## Testing

To launch your application's tests, run:

    ../gradlew test integrationTest

## Deployment

For instructions to set up BKP service, please refer to [quick start guide](./BKPS_QUICK_INSTALLATION_README.pdf).

This quick start guide describes the BKP service, its dependencies, and provides
example instructions to set up the BKP service. This quick start guide is not intended to
provide detailed security architecture information of the BKP service.
