# d2-web runtime image: the staged site (engine + client) served by play/serve.mjs. It carries NO game data.
# D2_ASSETS=byo (default): players choose their own MPQs in the browser. D2_ASSETS=hosted: mount D2 at /retail.
# Built by tools/docker-release.sh from build-wasm/site, minus vectors/ and widebg/ (retail-derived).
FROM node:22-alpine
WORKDIR /app
COPY --chown=node:node . /app
ENV D2_BIND=0.0.0.0 PORT=8080 D2_RETAIL_DIR=/retail D2_ASSETS=byo
EXPOSE 8080
USER node
ENTRYPOINT ["/bin/sh", "/app/docker-entrypoint.sh"]
