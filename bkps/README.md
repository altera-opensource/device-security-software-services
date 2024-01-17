# bkps

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

## Testing

To launch your application's tests, run:

    ./gradlew test

## Using Docker to simplify development (optional)

You can use Docker to improve your JHipster development experience. A number of docker-compose configuration are available in the [src/main/docker](src/main/docker) folder to launch required third party services.

For example, to start a postgresql database in a docker container, run:

    docker-compose -f src/main/docker/postgresql.yml up -d

To stop it and remove the container, run:

    docker-compose -f src/main/docker/postgresql.yml down

You can also fully dockerize your application and all the services that it depends on.
To achieve this, first build a docker image of your app by running:

    ./gradlew bootJar -Pprod buildDocker

Then run:

    docker-compose -f src/main/docker/app.yml up -d


### Build REST API HTML/PDF DOC

To build documentation use below command in terminal

```bash
../gradlew clean integrationTest --tests=DocumentationTest openApiGenerate asciidoctor
```
