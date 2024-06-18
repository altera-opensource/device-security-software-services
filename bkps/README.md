# Black Key Provisioning Service (BKPS)

The Black Key Provisioning Service is a suite of applications used to securely provision an AES root key to an Intel Stratix 10 or Intel Agilex Device.
Black key provisioning involves a multi-step process to establish a mutually authenticated secure connection between an SDM-based device at an untrusted manufacturing facility and an instance of the Intel-provided black key provisioning service that runs on a trusted system in a trusted location.
All connections between the Black Key Provisioning Service and the tools it relies on are authenticated connections using mTLS.
The mutually authenticated secure connection helps to ensure that the black key provisioning service can program the AES root key and other confidential information without exposing any data to an intermediate or third party.
This quick start guide describes the Black Key Provisioning Service, its dependencies, and provides example instructions to set up the Black Key Provisioning Service.

## Development

To start your application in the dev profile, simply run:

    ./gradlew

## Building for production

### Packaging as jar

To build the final jar and optimize application for production, run:

    ./gradlew -Pprod clean bootJar

To ensure everything worked, run:

    java -jar build/libs/*.jar

### Packaging as war

To package your application as a war in order to deploy it to an application server, run:

    ./gradlew -Pprod -Pwar clean bootWar

### Build SQL scheme file using Liquibase

To build SQL scheme file using Liquibase for production and AWS, run:

    ./gradlew -Pprod -Paws liquibaseGenerateSql -Pversion=${version} -Dversion=${version}

## Testing

To launch your application's tests, run:

    ./gradlew test

### Build REST API HTML/PDF DOC

To build documentation use below command in terminal

```bash
../gradlew clean integrationTest --tests=DocumentationTest openApiGenerate asciidoctor
```
