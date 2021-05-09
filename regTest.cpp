#include <pcrecpp.h>
#include <iostream>

int main() {
    std::string contents = "--------------------------eba4d02620bdb4f6\nContent-Disposition: form-data; name=\"ZIP\"; filename=\"h.png\"\nContent-Type: image/png\n\n--------------------------8c078fed966ff6fe\nContent-Disposition: form-data; name=\"ZIP\"; filename=\"tree-pack.xml\"\nContent-Type: application/xml\n\n<?xml version=\"1.0\"?>\n<Packages>\n  <Individual name=\"Designer\">\n    <Name>Designer</Name>\n    <Description>A BrAIn-API add-on that adds routes to help people design. This makes routes to generate colour palettes, generates fonts and even send previews of those to show how they look.</Description>\n    <ID></ID>\n    <FilePath>/packages/ID</FilePath>\n  </Individual>\n</Packages>\n\n--------------------------8c078fed966ff6fe--\n\n--------------------------eba4d02620bdb4f6--\n";
    pcrecpp::RE reg("-+.+\\nContent-Disposition: form-data; name=\"(\\w+| +)\"; filename=\"(.+)\"\\nContent-Type: (\\w+\\/\\w+)\\n\\n((.|\\n)+)\\n-+.+--.+|\\n+", pcrecpp::RE_Options()
    .set_caseless(true)
    .set_multiline(true));
    pcrecpp::StringPiece input(contents);
    int count = 0;
    std::string match;

    std::cout << "h" << contents << std::endl;
    /* for (int i=0;i<5;i++) {
        reg.FindAndConsume(&input, &match);
        std::cout << "regex match " << i << ": " << match << std::endl;
    } */
    while (reg.FindAndConsume(&input, &match)) {
        count++;
        std::cout << count << " " << match << std::endl;
    }
}