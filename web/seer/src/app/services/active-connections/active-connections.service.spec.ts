import { TestBed, inject } from '@angular/core/testing';

import { ActiveConnectionsService } from './active-connections.service';

describe('ActiveConnectionsService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [ActiveConnectionsService]
    });
  });

  it('should be created', inject([ActiveConnectionsService], (service: ActiveConnectionsService) => {
    expect(service).toBeTruthy();
  }));
});
